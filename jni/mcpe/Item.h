#pragma once

class Item {
public:

Item(int);
	char filler_item[76];

// static fields
	static Item* items[256];

	// static methods
	static void initItems();
};
