#include "TerrainGenerationScene.h"
#include "Data\ParseApplicationSettings.h"

namespace Engine
{
	TerrainGenerationScene::TerrainGenerationScene(IRenderer* renderer, WindowProperties& props, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour)
	{
		m_Renderer = renderer;
		m_AmbientColour = ambientColour;
		m_backgroundColour = backgroundColour;
		m_SpecularPower = specularPower;
		m_WindowProps = props;
	}
	//Function to setup all the geometry to be used in the scene
	bool TerrainGenerationScene::InitGeometry()
	{

		//Update the size of the HeightMap
		// --Has to be equal to 2^n + 1 in order for the Diamond Square algorithm to work on the HeightMap
		HeightMap.resize(SizeOfTerrain + 1, std::vector<float>(SizeOfTerrain + 1, 0));

		//Build the HeightMap with the value of 1
		BuildHeightMap(1);
		std::string path = "media/";

		m_SceneCamera = new Camera();


		m_EntityManager = new EntityManager(m_Renderer);

		transform.Scale = { 2.0f, 0.0f, 2.0f };
		//transform.Rotation = {gen::ToRadians(180.0f), 0.0f, 0.0f};

		m_EntityManager->CreateModelEntity("ground", path + "Ground.x", true, path + "Ground_Base.png", transform);

		m_SceneCamera->SetPosition({0.0f, 50.0f, 0.0f});
		m_SceneCamera->SetRotation({gen::ToRadians(180.0f),0.0f, 0.0f});

		return true;
	}

	//Function to setup the scene 
	bool TerrainGenerationScene::InitScene()
	{

		//Setting up the flags that will be used when creating the ImGui windows
		windowFlags |= ImGuiWindowFlags_NoScrollbar;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		windSize[0].width = 1600; windSize[0].height = 900; windSize[0].winString = std::to_string(windSize[0].width) + " x " + std::to_string(windSize[0].height);
		windSize[1].width = 1920; windSize[1].height = 1080; windSize[1].winString = std::to_string(windSize[1].width) + " x " + std::to_string(windSize[1].height);

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
			DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);

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

		m_SceneCamera->Control(frameTime);

		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);

			d11Renderer->GetRenderStats(frameTime, m_FPS, m_FrameTimeMS);
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		CVector2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			//ENGINE_CORE_TRACE("X = {0} Y = {1}", mouseX, mouseY);
		}

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

	void TerrainGenerationScene::SettingsMenu()
	{
		ImGuiWindowFlags SettingsWinFlags = 0;
		SettingsWinFlags = /*ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |*/ ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;
		ImGui::Begin("SettingsMenu", nullptr, SettingsWinFlags);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Window"))
			{
				ImGui::Text("Hello");
				ImGui::EndTabItem();
			}
			//if (ImGui::BeginTabItem("Camera"))
			//{
			//	
			//	ImGui::EndTabItem();
			//}
			ImGui::EndTabBar();
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
		static int n = 0;
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

				if (ImGui::MenuItem("Exit"))
					DestroyWindow(m_Renderer->GetWindowProperties().Hwnd);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::BeginMenu("Graphics"))
				{
					ImGui::Combo("Resolution", &n, " 1600 x 900\0 1920 x 1080\0\0");
					ImGui::Checkbox("VSync", &m_WindowProps.VSync);
					if (ImGui::Button("Apply"))
					{
						m_WindowProps.Height = windSize[n].height;
						m_WindowProps.Width = windSize[n].width;

						m_WindowProps.VSync = m_WindowProps.VSync;

						ParseApplicationSettings settings;
						settings.SaveWindowSettings(m_WindowProps);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("User Interface"))
				{

					if (ImGui::Button("Show ImGui demo"))
					{
						m_Showdemo = !m_Showdemo;
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Begin("Stats");
		ImGui::Text("Frame Time: %s ms", m_FrameTimeMS);
		ImGui::Text("Frames Per Second(FPS): %s", m_FPS);
		ImGui::Text("quads: %s", m_EntityManager->GetEntity("ground")->GetName().c_str());

		ImGui::Text("vertices: ");
		ImGui::Text("indices: ");

		ImGui::Text("Number of Entities in the Scene: %i", m_EntityManager->NumEntities());

		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", m_SceneCamera->Position().x, m_SceneCamera->Position().y, m_SceneCamera->Position().z);
		ImGui::Text("Camera Rotation: (%.2f, %.2f, %.2f)", m_SceneCamera->Rotation().x, m_SceneCamera->Rotation().y, m_SceneCamera->Rotation().z);

		TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("ground")->GetComponent("Transform"));

		ImGui::Text("Model Position: (%.2f, %.2f, %.2f)", comp->GetPosition().x, comp->GetPosition().y, comp->GetPosition().z);
		ImGui::Text("Model Rotation: (%.2f, %.2f, %.2f)", comp->GetRotation().x, comp->GetRotation().y, comp->GetRotation().z);

		std::string path = "media/";

		ImGui::SliderFloat3("Position", &transform.Position.x,-100.0f, 100.0f);
		ImGui::SliderFloat3("Scale", &transform.Scale.x,0, 100.0f);

		if (ImGui::Button("Create Entity"))
		{
			m_EntityManager->CreateModelEntity("Cube", path + "Teapot.x", true, path + "Rock.png", transform);
		}
		if (ImGui::Button("Create Light"))
		{
			m_EntityManager->CreateLightEntity("Light");
		}

		ImGui::End();

		if (m_Showdemo)
		{
			ImGui::ShowDemoWindow();
		}

		ImGui::Begin("Settings");
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport",0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		CVector2 m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };


		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);
			ImGui::Image(d11Renderer->GetSceneShaderResourceView(), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		}

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
		return m_WindowProps.VSync;
	}

}
