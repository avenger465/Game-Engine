#pragma once
#include "System\Interfaces\IScene.h"
#include "System\Interfaces\IRenderer.h"
#include "Renderer\DirectX11Renderer\Renderer.h"
#include "ECS\EntityManager.h"
#include "Math/CPerlinNoise.h"
#include "Math/DiamondSquare.h"
#include "Math/CVector3.h"
#include "Common\EngineProperties.h"
#include "Utility\GraphicsHelpers.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

namespace Engine
{

	class TerrainGenerationScene :
		public IScene
	{
		//----------------------//
		// Construction / Usage	//
		//----------------------//
	public:
		TerrainGenerationScene(IRenderer* renderer, CVector3 ambientColour,
			float specularPower, ColourRGBA backgroundColour);

		//The size of the HeightMap
		const int SizeOfTerrain = 256;

		//The Number of the vertices for the HeightMap
		const int SizeOfTerrainVertices = 255;

		//Function to setup all the geometry to be used in the scene
		virtual bool InitGeometry() override;

		//Function to setup the scene 
		virtual bool InitScene() override;

		// Release the geometry resources created above
		virtual void ReleaseResources() override;

		//Function to render scene from the Camera's perspective
		void RenderSceneFromCamera();

		//--------------------------------------------------------------------------------------
		// Scene Render and Update
		//--------------------------------------------------------------------------------------

		//Function to render the scene, called every frame
		virtual void RenderScene() override;

		// frameTime is the time passed since the last frame
		virtual void UpdateScene(float frameTime) override;

		void RenderSceneEntities();

		//Function to contain all of the ImGui code
		void IMGUI();

		// Returns the camera
		virtual Camera* GetCamera() override;

		// Scene Settings
		virtual void SetAmbientColour(CVector3 ambientColour) override;
		virtual void SetSpecularPower(float specularPower) override;
		virtual void SetBackgroundColour(ColourRGBA backgroundColour) override;
		virtual void SetVSync(bool VSync) override;

		virtual CVector3 GetAmbientColour() override;
		virtual float GetSpecularPower() override;
		virtual ColourRGBA GetBackgroundColour() override;
		virtual bool GetVSync() override;

		//--------------------------//
		// Private helper functions	//
		//--------------------------//
	private:

		//Building the HeightMap
		void BuildHeightMap(float height);

		//Function to build the height map with the Perlin Noise Algorithm
		void BuildPerlinHeightMap(float amplitude, float frequency, bool bBrownianMotion);

		//Perlin Noise with Octaves Function
		void PerlinNoiseWithOctaves(float Amplitude, float frequency, int octaves);

		//Rigid Noise Function
		void RigidNoise();

		//Inverse Rigid Noise Function
		void InverseRigidNoise();

		//Function to call the Diamond Sqaure Algorithm
		void DiamondSquareMap();

		//Terracing Function
		void Terracing(float terracingMultiplier);

		//Normalisation of the HeightMap
		void NormaliseHeightMap(float normaliseAmount);

		//Function to update the position of every plant in the scene
		void UpdateFoliagePosition();

		void SettingsMenu();

		EntityTransform transform = EntityTransform();

		//-------------//
		// Member data //
		//-------------//
	private:

		//HeightMap
		std::vector<std::vector<float>> HeightMap;

		//Original Position of the Camera
		CVector3 CameraPosition{ 5500.55f, 7602.11f, -7040.85f };
		CVector3 CameraRotation{ gen::ToRadians(34.3775f), 0.0f, 0.0f };

		//initial number of window flags for the ImGui Window
		ImGuiWindowFlags windowFlags = 0;

		//Min and Max point of the Grid Mesh
		CVector3 TerrainMeshMinPt = { 0,0,0 };
		CVector3 TerrainMeshMaxPt = { 1000,0,1000 };

		//Amount to normalise the HeightMap by
		float HeightMapNormaliseAmount = 2.0f;

		//Standard size of the ImGui Button
		ImVec2 ButtonSize = { 162, 20 };

		//Resolution of the HeightMap
		int resolution = 500;

		//Seed for the Perlin Noise Algorithm
		int seed = 0;

		bool m_Showdemo = false;

		//Variables that control the number of plants in the scene
		int plantResizeAmount = 2;
		int CurrentPlantVectorSize = 2;

		//Bools to enable or disable content
		bool enableTerrain = true;
		bool enableWireFrame = false;

		//Variables used when getting a Perlin Noise number
		float frequency = 0.125f;
		float Amplitude = 200.0;

		//Variables used for the Perlin Noise with octaves algorith,
		int octaves = 5;
		float AmplitudeReduction = 0.33f;
		float FrequencyMultiplier = 1.5f;

		//Amount to Terrace the terrain by
		float terracingMultiplier = 1.1f;

		//Spread used by the Diamond Square Algorithm
		float Spread = 30.0;
		float SpreadReduction = 2.0f;

		//Vector to scale the Terrain by
		CVector3 TerrainYScale = { 10, 30, 10 };


	private:

		struct SWinSize
		{
			int width;
			int height;
			std::string winString;
		};

		SWinSize windSize[2];
		const char* window[2];
		const char* windowPreviewValue;
		int currentWindowIndex = 0;

		IRenderer* m_Renderer;
		EntityManager* m_EntityManager = nullptr;
		WindowProperties m_WindowProps;

		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;

		Camera* m_SceneCamera = nullptr;
	};
}
