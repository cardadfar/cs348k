#Frame Smear Usage 

## Generate Optical Flow Map Video 

Open the dense_optical_flow_vis.py python script. Insert the path to the video you wish to generate optical flow for. Insert the path for the output optical
flow map video 

Run `python dense_optical_flow_vis.py` and the video should show up at the specified path. 

## Using the Plugin 

Load the Motion Blur AE plugin located in`~/assets/motion_blur`. Open AE and add both videos as separate layers in a composition.

Search for and click on the Motion Blur plugin. Apply the plugin to the layer with the optical flow map. Once the plugin loads, look at the selection menu for the "Layer" parameter and choose the layer with the original video.

The effect should appear. Use sliders to change parameters to tune the effect.  
