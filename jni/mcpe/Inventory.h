#pragma once
#include "ItemInstance.h"

class Inventory {
public:
	virtual void add(ItemInstance*);
	virtual void removeItemInstance(const ItemInstance*);
};