import numpy as np

"""This script implements the functions for data augmentation
and preprocessing.
"""

def parse_record(record, training):
	"""Parse a record to an image and perform data preprocessing.

	Args:
		record: An array of shape [3072,]. One row of the x_* matrix.
		training: A boolean. Determine whether it is in training mode.

	Returns:
		image: An array of shape [32, 32, 3].
	"""

	# Reshape from [depth * height * width] to [depth, height, width].
	depth_major = record.reshape((3, 32, 32))

	# Convert from [depth, height, width] to [height, width, depth]
	image = np.transpose(depth_major, [1, 2, 0])

	if not training:
		image = preprocess_image(image, training) # If any.
	
	return image


def preprocess_image(image, training):
	"""Preprocess a single image of shape [height, width, depth].

	Args:
		image: An array of shape [32, 32, 3].
		training: A boolean. Determine whether it is in training mode.

	Returns:
		image: An array of shape [32, 32, 3]. The processed image.
	"""
		
	mean = np.mean(image)
	std = np.std(image)
	image = (image - mean) / std

	return image