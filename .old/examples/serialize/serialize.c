#pragma once

#define SERIALIZE
#define ATTRIBUTE(attr)

#define serialize(type, value) _serialize_##type(value); printf("\n")

