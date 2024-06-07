import cv2 as cv 
import numpy as np 


# Insert path to video you wish to generate optical flow for
cap = cv.VideoCapture("frame_smear_videos/test_frame_smear_1.mp4")

# ret = a boolean return value from 
# getting the frame, first_frame = the 
# first frame in the entire video sequence 
ret, first_frame = cap.read() 

# Converts frame to grayscale because we 
# only need the luminance channel for 
# detecting edges - less computationally 
# expensive 
prev_gray = cv.cvtColor(first_frame, cv.COLOR_BGR2GRAY) 

# Creates an image filled with zero 
# intensities with the same dimensions 
# as the frame 
mask = np.zeros_like(first_frame) 

# Sets image saturation to maximum 
mask[..., 1] = 255

i = 0

# Output video
fourcc = cv.VideoWriter_fourcc(*'mp4v')
fps = cap.get(cv.CAP_PROP_FPS)
frame_size = (first_frame.shape[1], first_frame.shape[0])

# Insert path to output optical flow video here
video = cv.VideoWriter("frame_smear_videos/test_frame_smear_1_dense.mp4", fourcc, fps, frame_size)

while(cap.isOpened()):
	
	# ret = a boolean return value from getting 
	# the frame, frame = the current frame being 
	# projected in the video 
	ret, frame = cap.read() 
	
	# Opens a new window and displays the input 
	# frame 
	cv.imshow("input", frame) 
	
	# Converts each frame to grayscale - we previously 
	# only converted the first frame to grayscale 
	gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

	gray = cv.GaussianBlur(gray, (5, 5), 0)
    
	# Calculates dense optical flow by Farneback method
	flow = cv.calcOpticalFlowFarneback(prev_gray, gray, None, 0.5, 10, 15, 3, 5, 1.2, 0) 
	
	# Computes the magnitude and angle of the 2D vectors 
	magnitude, angle = cv.cartToPolar(flow[..., 0], flow[..., 1]) 
	# Sets image hue according to the optical flow
	# direction 
	mask[..., 0] = angle * 180 / np.pi / 2
	
	# Sets image value according to the optical flow 
	# magnitude (normalized) 
	mask[..., 2] = cv.normalize(magnitude, None, 0, 255, cv.NORM_MINMAX) 
	
	# Converts HSV to RGB (BGR) color representation 
	rgb = cv.cvtColor(mask, cv.COLOR_HSV2BGR) 
	
	# Opens a new window and displays the output frame
	#cv.imwrite(f"dense{i}.jpg", rgb)
	#cv.imwrite(f"normal{i}.jpg", frame)
	#cv.imshow("dense optical flow", rgb)
	filtered = cv.bilateralFilter(rgb, 50, 75, 75)

	video.write(filtered)

	# Updates previous frame
	prev_gray = gray

# The following frees up resources and 
# closes all windows 
cap.release()
video.release()
cv.destroyAllWindows()
