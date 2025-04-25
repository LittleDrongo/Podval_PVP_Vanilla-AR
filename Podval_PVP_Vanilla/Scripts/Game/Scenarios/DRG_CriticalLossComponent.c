class DRG_CriticalLossComponentClass : DRG_MissionModuleComponentClass
{
}

class DRG_CriticalLossComponent : DRG_MissionModuleComponent
{
	
	[Attribute(defvalue: "10", desc: "Advance game state after N second.", uiwidget: UIWidgets.EditBox, category: "Settings")]
	int m_iAdvanceAfterSec;
	
	[Attribute(defvalue: "3", desc: "Number of players participating in tests.", uiwidget: UIWidgets.EditBox, category: "Testing")]
	int m_iTestingPlayerCounts;
	
	[Attribute(defvalue: "true", desc: "Use TestingMode", uiwidget: UIWidgets.EditBox, category: "Testing")]
	bool m_bUseTestingMode;
	
	
	[Attribute(category: "Critical Losses Logic")]
	ref array<ref LossLogic> m_aLossLogics;		

	PS_GameModeCoop m_GameModeCoop;
		
	
	
	override void OnPostInit(IEntity owner){		
		if (!Replication.IsServer()){
			return;
		}
		
		m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());		
		DRG_MissionManagerComponent.BumpDescription();	
		GetGame().GetCallqueue().CallLater(handle, 500, true);
	
	};	
	
		
	
	override void FillDescription(out string desc){
		
		int counter;		
		
		desc = desc + "<color hex=\"0xFFE2A74F\">" +"Параметры завершения по потерям" + "<color name>\n";
		
		foreach (LossLogic lossLogic : m_aLossLogics){
			counter++;
									
			desc = desc + "Условие #" + counter.ToString()+"\n";
			
			foreach (FactionLoss lossFaction : lossLogic.m_aLosses) {
				
				FactionManager factionManager = GetGame().GetFactionManager();				
				Faction faction = factionManager.GetFactionByKey(lossFaction.m_sFactionKey);				
				string factionName = faction.GetFactionName();

				
				string playersWorld;
				string leftWorld;
				string countWorld = lossFaction.m_iCriticalLossCount.ToString();
				
				int count = lossFaction.m_iCriticalLossCount;
			
			
				if (count == 0)
				{				
					countWorld = "";				
					leftWorld = ": не осталось игроков";
					playersWorld = "";
				} else 				
				if (count == 1)
				{					
					leftWorld = ": остался ";
					playersWorld = " игрок";
				
				} else 
				if (count >= 2 && count <= 4)
				{
					leftWorld = ": осталось ";
					playersWorld = " игрока";
				} else				        
				{
				leftWorld = ": осталось ";
				      playersWorld = " игроков"; 
				}


				string line = "— " + factionName + leftWorld + countWorld + playersWorld + "\n";
				desc = desc + line;	
			}
			desc = desc + "\n";
		}	
		
	};
	
	
	void handle(){
		if (CheckIsFreezeTimeEnd()) {
			int playersOnStart = GetGame().GetPlayerManager().GetPlayerCount();
			
		
			if (!m_bUseTestingMode) {
				CheckLosses();
				HandleLogic();
			} else if (playersOnStart > m_iTestingPlayerCounts && m_bUseTestingMode) {
				CheckLosses();
				HandleLogic();
			} else {			
				NotifyPlayers("Режим тестирования сценария. Логика завершения по потерям выключена. Игроков должно быть > "+ m_iTestingPlayerCounts.ToString() + " сейчас " + playersOnStart);
				GetGame().GetCallqueue().Remove(handle);
			}
		}
	};
	
	
	bool CheckIsFreezeTimeEnd(){	
		if (!m_GameModeCoop)
		{
			m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());
			if (!m_GameModeCoop){		
				return false; // still not ready, try again next call
			};
		};
		
		return m_GameModeCoop.IsFreezeTimeEnd();
	};
	
	void CheckLosses(){		
		foreach (LossLogic lossLogic : m_aLossLogics){
			foreach (FactionLoss lossFaction : lossLogic.m_aLosses) {
				if (CountAliveCharacter(lossFaction.m_sFactionKey) <= lossFaction.m_iCriticalLossCount){
					lossFaction.m_iIsLossTaken = true;
				} else {
					lossFaction.m_iIsLossTaken = false; // Возможн стоит пересмотреть такой подход
				}			
			}
		}		
	};
	
	void HandleLogic(){
		
		foreach (LossLogic logic : m_aLossLogics){			
			if (logic.IsLossTaken()){			
				
				
				SwitchObjective(logic.m_sObjectiveNames, true);
					
				if (logic.m_bUseMessage){
				   NotifyPlayers(logic.m_bEventMessage);
				}
				
				if (logic.m_bAdvanceGameStage){
					GetGame().GetCallqueue().CallLater(AdvanceState, m_iAdvanceAfterSec*1000, false);	
				}
				
				GetGame().GetCallqueue().Remove(handle);
			}
		}
		
	};
	
	void AdvanceState(){	
		m_GameModeCoop.AdvanceGameState(SCR_EGameModeState.GAME);
	};
	
	
	int CountAliveCharacter(FactionKey factionKey)
	{
		int iAliveCount = 0;
		
 		PS_PlayableManager manager = PS_PlayableManager.GetInstance();		
 		array<PS_PlayableContainer> playables = manager.GetPlayablesSorted();		
		
		
		foreach(PS_PlayableContainer playable : playables)
		{			
			if (factionKey == playable.GetFactionKey() && playable.GetDamageState() != EDamageState.DESTROYED){
				iAliveCount++;				
			}
						
		}		
			
		return iAliveCount;	
	};
	
	void SwitchObjective(array<string> objectivesNames, bool flag){
		
		foreach (string objName : objectivesNames){
				PS_Objective objective = PS_Objective.Cast(GetGame().GetWorld().FindEntityByName(objName));		
		objective.SetCompleted(flag);
		};	
	};
}


[BaseContainerProps()]
class LossLogic
{
	//─────────[ FactionLoss ]───────────
	
	[Attribute(category: "Faction loss")]
	ref array<ref FactionLoss> m_aLosses;
	
	//─────────[ Objectives ]───────────
	
	[Attribute(defvalue: "", desc: "Specify the objective names to be switched in the debriefing section.", uiwidget: UIWidgets.EditBox)]
	ref array<string> m_sObjectiveNames; 
	
	[Attribute(defvalue: "false", desc: "Advanced game stage to AAR (Debrifieng)", uiwidget: UIWidgets.EditBox)]
	bool m_bAdvanceGameStage;
	
	//─────────[ Notifications ]───────────
	
    [Attribute(defvalue: "false", desc: "Is use notification message for this event.", uiwidget: UIWidgets.EditBox)]
	bool m_bUseMessage;
	
	[Attribute(defvalue: "Победа атаки: Оборона понесла серьёзные потери", desc: "Notification message.", uiwidget: UIWidgets.EditBoxMultiline)]
	string m_bEventMessage;
	
	//─────────[ Private Variable ]───────────
	
	bool IsLossTaken(){				
		foreach (FactionLoss loss : m_aLosses) {			
			if (loss.m_iIsLossTaken == false) {
				return false;
			}
		}
		
		return true;
	};	
}

[BaseContainerProps()]
class FactionLoss
{	
	[Attribute(defvalue: "5", desc: "Critical losses for the faction.", uiwidget: UIWidgets.EditBox, category: "Settings")]
	int m_iCriticalLossCount;
	
	[Attribute(defvalue: "USSR", desc: "For which faction does this rule apply.", uiwidget: UIWidgets.EditBox, category: "Settings")]
	FactionKey m_sFactionKey;
	
	bool m_iIsLossTaken;
}


void NotifyPlayers(string msg){
		
		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("smsg");
		
		invoker.Invoke(null, msg);
};	



