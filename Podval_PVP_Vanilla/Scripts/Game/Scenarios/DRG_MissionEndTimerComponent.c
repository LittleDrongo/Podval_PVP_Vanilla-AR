class DRG_MissionEndTimerComponentClass : DRG_MissionModuleComponentClass
{
}

class DRG_MissionEndTimerComponent : DRG_MissionModuleComponent
{
	[Attribute(defvalue: "3600", desc: "Time is given for the mission", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected int m_iMissionTime;

	[Attribute(defvalue: "Время на миссию вышло!", desc: "Time is given for the mission", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected string m_sMessage;
	
	[Attribute(defvalue:"", desc: "Notifications", category: "Mission second left")]
	protected ref array<int> m_aTimeLeftWarningTimers;
	
	[Attribute(defvalue: "Осталось времени на миссию %1", desc: "---", uiwidget: UIWidgets.EditBox, category: "Mission second left")]
	protected string m_sTimesLeftMessage;
		
	PS_GameModeCoop m_GameModeCoop;	
	
	override void OnPostInit(IEntity owner){	
		if (!Replication.IsServer()){
			return;
		}
		
		m_GameModeCoop = PS_GameModeCoop.Cast(GetGame().GetGameMode());	
		DRG_MissionManagerComponent.BumpDescription();	
		GetGame().GetCallqueue().CallLater(CheckIsFreezeTimeEnd, 500, true);	
			
	};	
	
 	override void FillDescription(out string desc){
				
		if (!m_bUseDescriptionGenerator){
			return;
		}
					
		int seconds = Math.Mod(m_iMissionTime, 60);
		int minutes = (m_iMissionTime / 60);
		int hours = (minutes / 60);
		minutes = Math.Mod(minutes, 60);
		string timeLeft = string.Format("%1:%2:%3", hours.ToString(2), minutes.ToString(2), seconds.ToString(2));
		
		desc = desc + "<color hex=\"0xFFE2A74F\">" +"Параметры завершения по времени" + "<color name>\n";
		desc = desc + "Время на миссию: " + timeLeft + "\n\n";
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
		
		AdvanceState();	
		
		
	};	
	
	void AdvanceState(){	
		m_GameModeCoop.AdvanceGameState(SCR_EGameModeState.GAME);
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
