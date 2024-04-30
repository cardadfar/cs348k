## Project Title

Spidervere Stylized Shaders

## Collaborators

Oscar Dadfar (odadfar), Olivia Loh (olivia77), Maxton Huff (maxton)

## Summary

We are going to write video shaders that simulate a lot of the stylized generative effects found in SpiderVerse and simmilar NPR media. We will start by trying to create the effects that effectively paramaeterize them, finding a balance between user control and usability without being too difficult. We will then shift gears to focus on performance, with the overall goal being a set of After Effects video filters that can be used by actual artists.

## Inputs & Outputs

We will constrin this problem to video only, meaning we want to be able to achieve normally 3D NPR effects on 2D video. Our input will be a video source and a few parameters yet to be determined, such as intensity of certain effects, color, etc. The output will also be a video, but with the filter applied.

## Task List

Our goal is to implement several of the following effects and focus on performance tuning.

### 1] Directional Motion Blur 
![IMG_2849](https://github.com/cardadfar/cs348k/assets/31673241/6cc37596-c1b6-417a-a496-6d0d8002d72b)
### 2] Frame Smearing
![INSIDER](https://github.com/cardadfar/cs348k/assets/31673241/c6d1df6f-42ea-4996-a0d3-2cf06a1b2471)
### 3] RGB Glitch
![IMG_1794](https://github.com/cardadfar/cs348k/assets/31673241/ce758691-fa7e-49ab-997a-f4d8c6a4179c)
### 4] Paper Cutout
![IMG_2648](https://github.com/cardadfar/cs348k/assets/31673241/33d8cb1f-6332-4ce0-b9ac-8f1a29f6cb09)
### 5] Stipple Shading
![image](https://github.com/cardadfar/cs348k/assets/31673241/b77aefc2-deb4-4be4-9e2e-5e7dc6512a84)
### 6] Sketch + Color Blotching
![maxresdefault](https://github.com/cardadfar/cs348k/assets/31673241/51e4c93b-8637-4c4e-9c9e-d051192fc39d)

Olivia:

Maxton:

Oscar: Goal (#4), nice-to-have (#6)
Steps (#4):
- Write a pixel-expander shader that uniformly expands segmented boundary
- Experiment with different variable-expansion rates:
- - Set different weights at regular-grid intervals that specify how expansive each region can be.
- - Identify sparse boundary points and tangents/normals, offset along tangents (creating implicit boundary). Perform point-in-boundary tests
- - Use perlin noise to offset uniform boundaries to create variable boundaries
- Tune performance:
- - What are the least amount of memory reads needed
- - What are the least amount of shader passes needed
- - Do we need the entire color frame? Or can we perform equally well with compressed imput? How does quality fall off?
 
Steps (#6):
- Get OpenGL set up in the Ae SDK for line drawing
- Identify high-contour regions to draw lines/strokes
- Create color histogram to reduce working colorset
- Tune performance:
- - What are the least amount of memory reads needed
- - What are the least amount of shader passes needed (lines, color, compositing)

## Deliverables & Evaluation

The result will be an After Effects plugin or suite of plugins (if we do multiple filters) built using the [AE SDK](https://ae-plugins.docsforadobe.dev/). We will do our prototyping using [ShaderToy](https://www.shadertoy.com/) and port the shadercode to After Effects for evaluation. After Effects comes with layer-based performance benchmarking, so applying our filters to an adjustment layer allows us to benchmark how expensive (memory/time) the filter is for varying video dimensions. Aside from performance, we also aim to measure our filters based on visual quality. Towards the end of the project, we will conduct surveys with video editors on how they interact and rate different features of the filters. We will real-time demo the filter(s) running in After Effects during demo day, as well as attach before-and-after results to our project paper.

## Risk

We can't imagine any noticable risks occuring. Risks such as "what if the performance is terrible and can't be made better" can be fixed by computing our shaders on lower resolution video. Because this is a generative/stylized effect, there isn't necessarily a quantitative ground truth that we are trying to target.

## Help

None that we are aware of.
