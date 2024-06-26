# OpenDirectXGraphicEngine
 Give me a window and give you a world.
# ODGE Information
- Don't forget to press "E" to open the UI control panel after running.  
- If you want to import a model, please ensure that its texture file has a dds replacement format. For example, if the model has a texture of Texture/baseColor1. 
png, the engine will search for Texture/baseColor1. dds.  Kind reminder, the resource directory contains the Microsoft texconv tool and instructions for use.
- We used third-party libraries Assimp and Dear IMGUI.  
- If you want to understand its implementation, you can refer to the "Overview of Graphics Engine Implementation" PPT file.
## Using this engine, you can achieve the following rendering effects 
The following models can be found in the published resources. The model name of Example 1 is "modern_city_block", Example 2 is "spartan_armour_mkv_ - _halo_reach", Example 3 is "reap_the_whirlwind". The textures of the models in the published resources all have corresponding. dds format maps, which means you can directly read them and render the following effects on your machine.
### Example 1: City model rendered with materials
 Use material preview mode to render the city model and compare the rendering results of Blender and OpenDirectX GraphicEngine.
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/a92b8bf9-cc2d-4050-9b28-52790d5136d2)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/d560d789-a942-44f7-8207-79b327441707)
### Example 2: A mecha model for lighting rendering
Use lighting mode to render the mecha model in a standing animation, in order to compare the realism of Blender and OpenDirectX GraphicEngine rendering PBR models.
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/bd86bc23-ff30-4549-a853-269c5cf32465)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/21dc83b5-878b-4ce3-ae03-4dbe60d6f55e)
### Example 3: Machine model for lighting rendering
The use of lighting mode to render machine models in heavily attacked animations was compared in terms of rendering performance and quality between Blender and OpenDirectX GraphicEngine. ODGE's support for wireframe mode, material rendering, and solid rendering was also demonstrated.
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/677bcb52-ca71-4d52-bb59-bc2f216e1e9a)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/e9d35f18-3a58-4f72-9dff-642251d4ad63)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/873e4990-625b-4372-98c7-2e72c5953d87)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/dc63bf5f-1b1b-4303-9173-3d13970603de)
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/8a4fae26-c71a-461c-97ef-c66dee8f43f7)
### Example 4: Multi animation file separation model for lighting rendering
The above models are all in the network universal model format GLTF. We are currently testing an FBX format model with bones, which does not come with animations. I have generated multiple animation files for it on the Mixamo website. After importing the model into ODGE, I tested loading multiple animations onto the model and successfully rendering and playing them.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/8b0fc1a0-c05b-4aeb-a380-b03c90649b35)  
Play dance animation.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/ba3cc26b-81a9-450a-9fa7-cea7dec79a24)  
Play cool animation.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/0620b760-e57c-4e07-9932-17e31fda3d85)  
Play street dance animation.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/f2387c93-eaad-47c1-bd53-851e31c1afec)  
Play whirlwind leg  animation.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/a52b9279-dce7-4d3b-9908-1bb1eb12f9d8)  
Play hook fist animation.  
![image](https://github.com/DU-GAN/OpenDirectXGraphicEngine/assets/165051859/04e2a154-9c28-4992-ba69-cbdfe1994b81)  

