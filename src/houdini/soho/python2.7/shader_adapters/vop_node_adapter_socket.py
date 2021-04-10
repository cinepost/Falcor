from enum import Enum
import hou

def toCamelCase(snake_str):
	if not snake_str:
		return snake_str

	snake_str = snake_str[0].lower() + snake_str[1:]

	components = snake_str.split('_')
	# We capitalize the first letter of each component except the first one
	# with the 'title' method and join them together.
	return components[0] + ''.join(x.title() for x in components[1:])

class VopNodeSocket(object):
	class Direction(Enum):
		INPUT 			= 1
		OUTPUT 			= 2
		INOUT 			= 3
			
	class DataType(Enum):
		UNDEF   = 0
		INT 	= 1
		FLOAT 	= 2
		STRING 	= 3
		COLOR   = 4
		VECTOR 	= 5
		VECTOR4 = 6
		MATRIX  = 7
		MATRIX3 = 8
		BSDF    = 9
		SHADER  = 10
		SURFACE = 11

		@classmethod
		def isSlandDataType(cls, data_type):
			if data_type == cls.STRING: return False

			return True

	_direction = Direction.INPUT

	def __init__(self, socket_name, socket_data_type_string, direction):
		self._vop_name = socket_name
		self._data_type = VopNodeSocket.dataTypeFromString(socket_data_type_string)
		self._direction = direction

	@property
	def vopName(self):
		return self._vop_name

	@property
	def codeVarName(self):
		return "_%s" % toCamelCase(self.vopName)
	
	@property
	def dataType(self):
		return self._data_type

	@property
	def direction(self):
		return self._direction

	@property
	def slangTypeAccessString(self):	
		if self._direction == VopNodeSocket.Direction.INPUT: return None
		if self._direction == VopNodeSocket.Direction.OUTPUT: return "out"
		if self._direction == VopNodeSocket.Direction.INOUT: return "inout"

	@property
	def slangDataTypeString(self):
		if self._data_type == VopNodeSocket.DataType.INT:    return 'int'
		if self._data_type == VopNodeSocket.DataType.FLOAT:  return 'float'
		if self._data_type == VopNodeSocket.DataType.VECTOR: return 'float3'
		if self._data_type == VopNodeSocket.DataType.VECTOR4:return 'float4'
		if self._data_type == VopNodeSocket.DataType.BSDF:   return 'bsdf'
		if self._data_type == VopNodeSocket.DataType.SHADER: return 'shader'
		if self._data_type == VopNodeSocket.DataType.SURFACE:return 'surface'
		
		if self._data_type == VopNodeSocket.DataType.STRING:
			raise ValueError('Con not convert VopNodeSocket.DataType.STRING to Slang type !!!')

		return 'undef'

	@classmethod
	def dataTypeFromString(cls, socket_data_type_string):
		if socket_data_type_string == 'undef': return VopNodeSocket.DataType.UNDEF
		if socket_data_type_string == 'int': return VopNodeSocket.DataType.INT
		if socket_data_type_string == 'float': return VopNodeSocket.DataType.FLOAT
		if socket_data_type_string == 'string': return VopNodeSocket.DataType.STRING
		if socket_data_type_string == 'vector': return VopNodeSocket.DataType.VECTOR
		if socket_data_type_string == 'color': return VopNodeSocket.DataType.VECTOR
		if socket_data_type_string == 'vector4': return VopNodeSocket.DataType.VECTOR4
		if socket_data_type_string == 'bsdf': return VopNodeSocket.DataType.BSDF
		if socket_data_type_string == 'shader': return VopNodeSocket.DataType.SHADER
		if socket_data_type_string == 'surface': return VopNodeSocket.DataType.SURFACE

		raise ValueError('Unknown socket data type %s', socket_data_type_string)