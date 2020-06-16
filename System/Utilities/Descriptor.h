#pragma once

template<typename Type, typename Desc>
class Descriptor
{
public:
	Descriptor(Type owner)
	{
		pDescription = owner::GetDesc();
	}

	Desc* pDescription;

};