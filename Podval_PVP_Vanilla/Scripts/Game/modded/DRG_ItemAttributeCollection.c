modded class SCR_ItemAttributeCollection
{	
	void SCR_ItemAttributeCollection(){		
		GetGame().GetCallqueue().Call(NoDraggable);
	}
	
	// Метод удаляет возможность перетаскивать предметы из любой техники.
	void NoDraggable(){
		m_bDraggable = false;
	}
};
