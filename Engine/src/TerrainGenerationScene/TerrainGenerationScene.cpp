#include "TerrainGenerationScene.h"

namespace Engine
{
	TerrainGenerationScene::TerrainGenerationScene(IRenderer* renderer, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour)
	{
		m_Renderer = renderer;
		m_AmbientColour = ambientColour;
		m_backgroundColour = backgroundColour;
		m_SpecularPower = specularPower;
		m_WindowProps = m_Renderer->GetWindowProperties();
	}
	//Function to setup all the geometry to be used in the scene
	bool TerrainGenerationScene::InitGeometry()
	{

		//Update the size of the HeightMap
		// --Has to be equal to 2^n + 1 in order for the Diamond Square algorithm to work on the HeightMap
		HeightMap.resize(SizeOfTerrain + 1, std::vector<float>(SizeOfTerrain + 1, 0));

		//Build the HeightMap with the value of 1
		BuildHeightMap(1);

		m_SceneCamera = new Camera();


		m_EntityManager = new EntityManager(m_Renderer);

		return true;
	}

	//Function to setup the scene 
	bool TerrainGenerationScene::InitScene()
	{

		//Setting up the flags that will be used when creating the ImGui windows
		windowFlags |= ImGuiWindowFlags_NoScrollbar;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		m_SceneCamera->SetPosition(CameraPosition);
		m_SceneCamera->SetRotation(CameraRotation);

		return true;
	}

	void TerrainGenerationScene::ReleaseResources()
	{
	}

	//Function to render scene from the Camera's perspective
	void TerrainGenerationScene::RenderSceneFromCamera()
	{

		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			Renderer* d11Renderer = static_cast<Renderer*>(m_Renderer);

			// Set camera matrices in the constant buffer and send over to GPU
			d11Renderer->PerFrameConstants.viewMatrix = m_SceneCamera->ViewMatrix();
			d11Renderer->PerFrameConstants.projectionMatrix = m_SceneCamera->ProjectionMatrix();
			d11Renderer->PerFrameConstants.viewProjectionMatrix = m_SceneCamera->ViewProjectionMatrix();
			UpdateConstantBuffer(d11Renderer->GetDeviceContext(), d11Renderer->PerFrameConstantBuffer, d11Renderer->PerFrameConstants);

			// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
			d11Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &d11Renderer->PerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
			d11Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &d11Renderer->PerFrameConstantBuffer);
		}

	}

	void TerrainGenerationScene::RenderSceneEntities()
	{
		m_EntityManager->RenderAllEntities();
		IMGUI();
	}

	//Function to render the scene, called every frame
	void TerrainGenerationScene::RenderScene()
	{

	}

	//Updating the scene, called every frame
	void TerrainGenerationScene::UpdateScene(float frameTime)
	{
		m_EntityManager->UpdateAllEntities(frameTime);
		// Control camera (will update its view matrix)
		//MainCamera->Control(frameTime);
		//GroundModel->SetScale(TerrainYScale);
	}

	//Building the HeightMap
	void TerrainGenerationScene::BuildHeightMap(float height)
	{
		//Loop through the HeightMap and set each value to the chosen height value
		for (int i = 0; i <= SizeOfTerrain; ++i) {
			for (int j = 0; j <= SizeOfTerrain; ++j) {
				HeightMap[i][j] = height;
			}
		}
	}

	//Normalisation of the HeightMap
	void TerrainGenerationScene::NormaliseHeightMap(float normaliseAmount)
	{
		//Loop through the HeightMap and divide each value by the normalisation amount
		for (int i = 0; i <= SizeOfTerrain; ++i) {
			for (int j = 0; j <= SizeOfTerrain; ++j) {
				HeightMap[i][j] /= normaliseAmount;
			}
		}
	}

	//Function to build the height map with the Perlin Noise Algorithm
	void TerrainGenerationScene::BuildPerlinHeightMap(float amplitude, float frequency, bool bOctaves)
	{
		//get the scale to make sure that the terrain looks consistent 
		const float scale = (float)resolution / (float)SizeOfTerrain;

		//Create a PerlinNoise object to get the Perlin Noise values
		CPerlinNoise* pn = new CPerlinNoise(seed);

		for (int z = 0; z <= SizeOfTerrain; ++z) // loop through the z 
		{
			for (int x = 0; x <= SizeOfTerrain; ++x) //loop through the x
			{
				//Get the current X and Z coordinates 
				double ZCoord = z * frequency * scale / 20;
				double XCoord = x * frequency * scale / 20;

				//get the Current Perlin Noise value
				float noiseValue = (float)pn->noise(XCoord, 0.0f, ZCoord) * amplitude;

				//Check if Perlin with Octaves has been selected and add it to the current HeightMap value if it has
				//Otherwise just update the HeightMap value to the new noiseValue
				if (bOctaves)
				{
					HeightMap[z][x] += noiseValue;
				}
				else
				{
					HeightMap[z][x] = noiseValue;
				}
			}
		}
	}

	//Perlin Noise with Octaves Function
	void TerrainGenerationScene::PerlinNoiseWithOctaves(float Amplitude, float frequency, int octaves)
	{
		//loop through the number of octaves
		for (int i = 0; i < octaves; ++i)
		{
			//Update the HeightMap with the Perlin noise function using the current Amplitude and Frequency
			BuildPerlinHeightMap(Amplitude, frequency, true);

			//Update the Amplitude and Frequency variables
			Amplitude *= AmplitudeReduction;
			frequency *= FrequencyMultiplier;
		}
	}

	//Rigid Noise Function
	void TerrainGenerationScene::RigidNoise()
	{
		//get the scale to make sure that the terrain looks consistent 
		const float scale = (float)resolution / (float)SizeOfTerrain;

		//Create a PerlinNoise object to get the Perlin Noise values
		CPerlinNoise* pn = new CPerlinNoise(seed);

		for (int z = 0; z <= SizeOfTerrain; ++z) // loop through the z
		{
			for (int x = 0; x < SizeOfTerrain; ++x) //loop through the x
			{
				//Get the current X and Z coordinates 
				double XCoord = x * frequency * scale / 20;
				double ZCoord = z * frequency * scale / 20;

				//Get the absolute value of the perlin noise function and subtract it from 1
				float value = (1.0f - abs((float)pn->noise(XCoord, 0.0f, ZCoord) * (float)Amplitude));

				//Then add the negated value to the current HeighMap value
				HeightMap[z][x] += -value;
			}
		}
	}

	//Inverse Rigid Noise Function
	void TerrainGenerationScene::InverseRigidNoise()
	{
		//get the scale to make sure that the terrain looks consistent 
		const float scale = (float)resolution / (float)SizeOfTerrain;

		//Create a PerlinNoise object to get the Perlin Noise values
		CPerlinNoise* pn = new CPerlinNoise(seed);

		for (int z = 0; z <= SizeOfTerrain; ++z) // loop through the z
		{
			for (int x = 0; x <= SizeOfTerrain; ++x) //loop through the x
			{
				//Get the current X and Z coordinates 
				double currentX = x * frequency * scale / 20;
				double currentZ = z * frequency * scale / 20;

				//Get the absolute value of the perlin noise function and subtract it from 1
				float value = (1.0f - abs((float)pn->noise(currentX, 0.0f, currentZ) * (float)Amplitude));

				//Then add the value to the current HeighMap value
				HeightMap[z][x] += value;
			}
		}
	}

	//Function to call the Diamond Sqaure Algorithm
	void TerrainGenerationScene::DiamondSquareMap()
	{
		//Create a new DiamondSqaure object with the required information
		DiamondSquare ds(SizeOfTerrain, Spread, SpreadReduction);

		//perform the Diamond Square Algorithm on the HeightMap
		ds.process(HeightMap);
	}

	//Terracing Function
	void TerrainGenerationScene::Terracing(float terracingMultiplier)
	{
		for (int x = 0; x <= SizeOfTerrain; ++x) // loop through the x
		{
			for (int z = 0; z <= SizeOfTerrain; ++z) //loop through the z
			{
				//Get the current height value
				float MapValue = HeightMap[x][z];

				//Round this height value
				float roundedValue = round(MapValue);

				//and then set the height to the rounded value divided by the multiplier
				HeightMap[x][z] = roundedValue / terracingMultiplier;
			}
		}
	}

	//Function to contain all of the ImGui code
	void TerrainGenerationScene::IMGUI()
	{

		//---------------------------------------------------------------------------------------------------------------------//
		//First ImGui window created is a dock taken from the docking example in imgui_demo.cpp from the ImGui examples library//
		//---------------------------------------------------------------------------------------------------------------------//
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		
		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					std::cout << "New";
					//NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					std::cout << "Open";
					//OpenScene();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					std::cout << "Save";
					//SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					std::cout << "Save as";
					//SaveSceneAs();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Begin("Stats");
		ImGui::Text("Draw Calls: ");
		ImGui::Text("quads: ");
		ImGui::Text("vertices: ");
		ImGui::Text("indices: ");
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		CVector2 m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };


		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			Renderer* d11Renderer = static_cast<Renderer*>(m_Renderer);
			ImGui::Image(d11Renderer->GetSceneShaderResourceView(), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		}

		//New ImGui window to contain the current scene rendered to a 2DTexture//
		//{
		//	ImGui::Begin("Scene", 0, windowFlags);
		//	ImGui::Image(SceneTextureSRV, ImVec2((float)textureWidth, (float)textureHeight));
		//	ImGui::End();
		//}

		//End the ImGui window
		ImGui::End();
		ImGui::PopStyleVar();


		ImGui::End();
	}

	Camera* TerrainGenerationScene::GetCamera()
	{
		return m_SceneCamera;
	}

	void TerrainGenerationScene::SetAmbientColour(CVector3 ambientColour)
	{
		m_AmbientColour = ambientColour;
	}

	void TerrainGenerationScene::SetSpecularPower(float specularPower)
	{
		m_SpecularPower = specularPower;
	}

	void TerrainGenerationScene::SetBackgroundColour(ColourRGBA backgroundColour)
	{
		m_backgroundColour = backgroundColour;
	}

	void TerrainGenerationScene::SetVSync(bool VSync)
	{
	}

	CVector3 TerrainGenerationScene::GetAmbientColour()
	{
		return m_AmbientColour;
	}

	float TerrainGenerationScene::GetSpecularPower()
	{
		return m_SpecularPower;
	}

	ColourRGBA TerrainGenerationScene::GetBackgroundColour()
	{
		return m_backgroundColour;
	}

	bool TerrainGenerationScene::GetVSync()
	{
		return false;
	}

}
