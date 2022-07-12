#pragma once
#include "BasicScene\Camera.h"
#include "Utility\ColourRGBA.h"

namespace Engine
{
	class IScene
	{
	public:
		virtual ~IScene() {};

		virtual bool InitGeometry() = 0;

		// Initializes the scene
		virtual bool InitScene() = 0;

		// Renders the scene
		virtual void RenderScene() = 0;

		// Updates the scene
		virtual void UpdateScene(float frameTime) = 0;

		// Releases the resource of the scene
		virtual void ReleaseResources() = 0;

		// Returns the camera
		virtual Camera* GetCamera() = 0;

		// Scene Settings
		virtual void SetAmbientColour(CVector3 ambientColour) = 0;
		virtual void SetSpecularPower(float specularPower) = 0;
		virtual void SetBackgroundColour(ColourRGBA backgroundColour) = 0;
		virtual void SetVSync(bool VSync) = 0;

		virtual CVector3 GetAmbientColour() = 0;
		virtual float GetSpecularPower() = 0;
		virtual ColourRGBA GetBackgroundColour() = 0;
		virtual bool GetVSync() = 0;
	};
}
