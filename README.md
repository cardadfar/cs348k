## Project Title

Stylized Shaders

## Collaborators

Oscar Dadfar (odadfar), Olivia Loh (pending), Maxton Huff (pending)

## Summary

We are going to write video shaders that simulate a lot of the stylized generative effects found in SpiderVerse and simmilar NPR media. We will start by trying to create the effects that effectively paramaeterize them, finding a balance between user control and usability without being too difficult. We will then shift gears to focus on performance, with the overall goal being a set of After Effects video filters that can be used by actual artists.

## Inputs & Outputs

We will constrin this problem to video only, meaning we want to be able to achieve normally 3D NPR effects on 2D video. Our input will be a video source and a few parameters yet to be determined, such as intensity of certain effects, color, etc. The output will also be a video, but with the filter applied.

## Task List

## Deliverables & Evaluation

The result will be an After Effects plugin or suite of plugins (if we do multiple filters) built using the [AE SDK](https://ae-plugins.docsforadobe.dev/). We will do our prototyping using [ShaderToy](https://www.shadertoy.com/) and port the shadercode to After Effects for evaluation. After Effects comes with layer-based performance benchmarking, so applying our filters to an adjustment layer allows us to benchmark how expensive (memory/time) the filter is for varying video dimensions. Aside from performance, we also aim to measure our filters based on visual quality. Towards the end of the project, we will conduct surveys with video editors on how they interact and rate different features of the filters. We will real-time demo the filter(s) running in After Effects during demo day, as well as attach before-and-after results to our project paper.

## Risk

We can't imagine any noticable risks occuring. Risks such as "what if the performance is terrible and can't be made better" can be fixed by computing our shaders on lower resolution video. Because this is a generative/stylized effect, there isn't necessarily a quantitative ground truth that we are trying to target.

## Help

None that we are aware of.
