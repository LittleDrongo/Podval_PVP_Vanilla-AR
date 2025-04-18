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
	
	[Attribute(defvalue:"", desc: "Notifications", category: "Mission second left")]
	protected ref array<int> m_aTimeLeftWarningTimers;
	
	[Attribute(defvalue: "Время на миссию %1", desc: "---", uiwidget: UIWidgets.EditBox, category: "Mission second left")]
	protected string m_sTimesLeftMessage;
		
	PS_GameModeCoop m_GameModeCoop;	
	
	override void OnPostInit(IEntity owner){	
		if (!Replication.IsServer()){
			return;
		}
		
		m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		GetGame().GetCallqueue().Call(GenerateMissionDesc);	
		GetGame().GetCallqueue().CallLater(CheckIsFreezeTimeEnd, 500, true);	
			
	};	
	
 	void GenerateMissionDesc(){
		
		protected ResourceName m_rStartLayout = "{41DAF7B7061DC0BC}UI/MissionDescription/DescriptionEditable.layout";		
		Resource descResource = Resource.Load("{3136BE42592F3B1B}PrefabsEditable/MissionDescription/EditableMissionDescription.et");		
		PS_MissionDescription m_MissionDescriptionTime = PS_MissionDescription.Cast(GetGame().SpawnEntityPrefab(descResource));

		m_MissionDescriptionTime.SetTitle("Завершение по времении");
		m_MissionDescriptionTime.SetVisibleForEmptyFaction(true);		
		m_MissionDescriptionTime.m_bShowForAnyFaction = true;
		m_MissionDescriptionTime.RegisterToDescriptionManager();		
		m_MissionDescriptionTime.SetLayout(m_rStartLayout);
		m_MissionDescriptionTime.m_iOrder = 100;
		
		
		int seconds = Math.Mod(m_iMissionTime, 60);
		int minutes = (m_iMissionTime / 60);
		int hours = (minutes / 60);
		minutes = Math.Mod(minutes, 60);
		string timeLeft = string.Format("%1:%2:%3", hours.ToString(2), minutes.ToString(2), seconds.ToString(2));
		
		string text = "Внимание! В миссии присутствует модуль завершения по времени";
		text = text + "\n\nВремя на миссию: " + timeLeft;
	
		
		m_MissionDescriptionTime.SetTextData(text);
    };
	
	
	void CheckIsFreezeTimeEnd(){	
	if (!m_GameModeCoop)
	{
		m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if (!m_GameModeCoop)
			return; // still not ready, try again next call
	}

	if (m_GameModeCoop.IsFreezeTimeEnd()) {
		GetGame().GetCallqueue().Remove(CheckIsFreezeTimeEnd);
		GetGame().GetCallqueue().CallLater(OnMissionTimeEnd, (m_iMissionTime * 1000), false);		
		
		foreach (int time : m_aTimeLeftWarningTimers){
			GetGame().GetCallqueue().CallLater(NotifyPlayersAboutTimeLeft, ((m_iMissionTime - time) * 1000), false, time);		
			}	
		}	
	};
	
	void OnMissionTimeEnd(){
		NotifyPlayersMissionEnd();		
	};
	
	
	void NotifyPlayersMissionEnd(){
		
		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		ChatCommandInvoker invoker = chatPanelManager.GetCommandInvoker("smsg");
		string message;
		
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
