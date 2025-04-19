[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class DRG_MissionManagerComponentClass : ScriptComponentClass
{
}

class DRG_MissionManagerComponent : ScriptComponent
{

	PS_MissionDescription m_MissionDescription;

	static DRG_MissionManagerComponent s_Instance;

	ref set<DRG_MissionModuleComponent> m_MissionModules = new set<DRG_MissionModuleComponent>();

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		s_Instance = this;
	}

	void RegisterModule(DRG_MissionModuleComponent module){
		m_MissionModules.Insert(module);
	}

	static DRG_MissionManagerComponent GetInstance()
	{
		return s_Instance;
	}

	static void BumpDescription(){
		GetGame().GetCallqueue().Remove(FillDescription);
		GetGame().GetCallqueue().Call(FillDescription);
	}

	static void FillDescription(){
		string description = "";

		s_Instance.CreateDescription();

		foreach (DRG_MissionModuleComponent module : s_Instance.m_MissionModules)
		{
			module.FillDescription(description);
		}

		s_Instance.m_MissionDescription.SetTextData(description);
	}


	void CreateDescription(){

		if (m_MissionDescription){
			return;
		}


		ResourceName startLayout = "{41DAF7B7061DC0BC}UI/MissionDescription/DescriptionEditable.layout";
		Resource descResource = Resource.Load("{3136BE42592F3B1B}PrefabsEditable/MissionDescription/EditableMissionDescription.et");
		m_MissionDescription = PS_MissionDescription.Cast(GetGame().SpawnEntityPrefab(descResource));

		m_MissionDescription.SetTitle("Параметры миссии");
		m_MissionDescription.SetVisibleForEmptyFaction(true);
		m_MissionDescription.RegisterToDescriptionManager();
		m_MissionDescription.SetLayout(startLayout);

		m_MissionDescription.SetShowForAnyFaction(true);
		m_MissionDescription.SetOrder(200);

	}
}
