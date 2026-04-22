import ctypes

# Load the shared library
my_lib = ctypes.CDLL('path_to_your_library.so')

# Define the argument and return types
my_lib.my_function.argtypes = [ctypes.c_int, ctypes.c_double]
my_lib.my_function.restype = ctypes.c_double

# Call the function
result = my_lib.my_function(5, 3.14)
print(result)