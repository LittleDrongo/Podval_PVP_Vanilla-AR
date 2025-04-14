class DRG_MissionEndTimerComponentClass : ScriptComponentClass
{
}

class DRG_MissionEndTimerComponent : ScriptComponent
{
	[Attribute(defvalue: "3600", desc: "Time is given for the mission", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected int m_iMissionTime;

	[Attribute(defvalue: "Время на миссию вышло!", desc: "Time is given for the mission", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected string m_sMessage;
	
	[Attribute(desc: "Objectives names which need to be switched", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected ref array<string> m_aObjectiveNames;
	
	/*
	[Attribute(defvalue:"", desc: "List of winning factions", category: "Settings")]
	protected ref array<FactionKey> m_aWinnersFactions;

	[Attribute(defvalue:"", desc: "List of losers factions", category: "Settings")]
	protected ref array<FactionKey> m_aLosersFactions;
	*/
	[Attribute(defvalue:"", desc: "Notifications", category: "Mission second left")]
	protected ref array<int> m_aTimeLeftWarningTimers;
	
	[Attribute(defvalue: "Время на миссию %1", desc: "---", uiwidget: UIWidgets.EditBox, category: "Mission second left")]
	protected string m_sTimesLeftMessage;
		
	PS_GameModeCoop m_GameModeCoop;	
	
	override void OnPostInit(IEntity owner){	
		if (Replication.IsServer()){
			m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());
			GetGame().GetCallqueue().CallLater(CheckIsFreezeTimeEnd, 500, true);	
		};		
	};	
	
	void CheckIsFreezeTimeEnd(){		
		if (m_GameModeCoop.IsFreezeTimeEnd()) {
			GetGame().GetCallqueue().Remove(CheckIsFreezeTimeEnd);
			GetGame().GetCallqueue().CallLater(OnMissionTimeEnd, (m_iMissionTime * 1000), false);		
			
			foreach (int time : m_aTimeLeftWarningTimers){
				GetGame().GetCallqueue().CallLater(NotifyPlayersAboutTimeLeft, ((m_iMissionTime - time) * 1000), false, time);		
			}	
		};	
	};
	
	void OnMissionTimeEnd(){
		NotifyPlayersMissionEnd();		
	};
	
	
	void NotifyPlayersMissionEnd(){
		
		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("smsg");
		string message;
		/*
		string winnersString;
		foreach (FactionKey factionKey : m_aWinnersFactions){
			winnersString = (winnersString + " " + factionKey);
		};
		
		string losersString;
		foreach (FactionKey factionKey : m_aLosersFactions){
			
			FactionManager factionManager = GetGame().GetFactionManager();
			
			Faction faction = factionManager.GetFactionByKey(factionKey);
			
			losersString = (losersString + " " + faction.GetFactionName());
		};
		*/
		message = (m_sMessage);
		
		invoker.Invoke(null, message);
		
		// Меняет у Objective статус
		foreach (string objectiveName : m_aObjectiveNames){
			PS_Objective objective = PS_Objective.Cast(GetGame().GetWorld().FindEntityByName(objectiveName));		
			objective.SetCompleted(true);
		};		
		
		
	};	
	
	void NotifyPlayersAboutTimeLeft(int timeSeconds){
		
		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("smsg");
		string message;
		
		int seconds = Math.Mod(timeSeconds, 60);
		int minutes = (timeSeconds / 60);
		int hours = (minutes / 60);
		minutes = Math.Mod(minutes, 60);
		string timeLeft = string.Format("%1:%2:%3", hours.ToString(2), minutes.ToString(2), seconds.ToString(2));
		
		message = string.Format(m_sTimesLeftMessage, timeLeft);
		
		invoker.Invoke(null, message);
		
	};
}
