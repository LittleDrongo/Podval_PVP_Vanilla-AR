class DRG_CriticalLossComponentClass : ScriptComponentClass
{
}

class DRG_CriticalLossComponent : ScriptComponent
{
	[Attribute(category: "Critical Losses Logic")]
	protected ref array<ref LossLogic> m_aLossLogics;			
	
	PS_GameModeCoop m_GameModeCoop;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}

[BaseContainerProps()]
class LossLogic
{
	//─────────[ FactionLoss ]───────────
	
	[Attribute(category: "Faction loss")]
	protected ref array<ref FactionLoss> m_aLosses;
	
	//─────────[ Objectives ]───────────
	
	[Attribute(defvalue: "", desc: "Specify the objective names to be switched in the debriefing section.", uiwidget: UIWidgets.EditBox)]
	protected array<string> m_sObjectiveNames;
	
	[Attribute(defvalue: "false", desc: "Advanced game stage to AAR (Debrifieng)", uiwidget: UIWidgets.EditBox)]
	protected bool m_bAdvanceGameStage;
	
	//─────────[ Notifications ]───────────
	
    [Attribute(defvalue: "false", desc: "Is use notification message for this event.", uiwidget: UIWidgets.EditBox)]
	protected bool m_bUseMessage;
	
	[Attribute(defvalue: "Победа атаки: Оборона понесла серьёзные потери", desc: "Notification message.", uiwidget: UIWidgets.EditBoxMultiline)]
	protected string m_bEventMessage;
	
	//─────────[ Private Variable ]───────────
}

[BaseContainerProps()]
class FactionLoss
{	
	[Attribute(defvalue: "5", desc: "Critical losses for the faction.", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected int m_iCriticalLossCount;
	
	[Attribute(defvalue: "USSR", desc: "For which faction does this rule apply.", uiwidget: UIWidgets.EditBox, category: "Settings")]
	protected FactionKey m_sFactionKey;
}
