
In Zephyr we can not import all boards (like this is done e.g for Arduino), because this would lead to invalid references to the device tree.

So we must only import the specific board separately.