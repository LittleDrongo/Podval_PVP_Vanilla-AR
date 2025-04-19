[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class DRG_MissionModuleComponentClass : ScriptComponentClass
{
}

class DRG_MissionModuleComponent : ScriptComponent
{
	[Attribute(defvalue: "true", desc: "", uiwidget: UIWidgets.EditBox, category: "Mission Module Description")]
	bool m_bUseDescriptionGenerator;
	
	void DRG_MissionModuleComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		ScriptCallQueue callQueue = GetGame().GetCallqueue();
		if (!callQueue)
			return;
		
		callQueue.Call(RegisterSelf);	
	}
	
	void RegisterSelf(){
		DRG_MissionManagerComponent.GetInstance().RegisterModule(this);
	}
	
	void FillDescription(out string desc);
}
