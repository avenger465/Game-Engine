#pragma once
#include "System\Interfaces\IState.h"

namespace Engine
{
	enum class ESamplingState
	{
		PointSampler = 0,
		TrilinearSampler,
		Anisotropic4xSampler,
		BilinearMirrorSampler
	};

	enum class EBlendingState
	{
		NoBlendingState = 0,
		AdditiveBlendingState,
		MultiplicativeBlendingState,
		AlphaBlendingState
	};

	enum class ERasterizerState
	{
		CullBackState = 0,
		CullFrontState,
		CullNoneState,
		WireFrameState
	};

	enum class EDepthStencilState
	{
		UseDepthBufferState = 0,
		DepthReadOnlyState,
		NoDepthBufferState
	};

	class DirectX11States : public IState
	{
	public:

		~DirectX11States();

		virtual bool InitStates(IRenderer* renderer) override;

		virtual void ReleaseStates() override;

		ID3D11SamplerState* GetSamplerState(ESamplingState ss); // Returns the Sampler state 
		ID3D11BlendState* GetBlendState(EBlendingState bs); // Returns the Blend state 
		ID3D11RasterizerState* GetRasterizerState(ERasterizerState rs); // Returns the Rasterizer state 
		ID3D11DepthStencilState* GetDepthStencilState(EDepthStencilState dss); // Returns the Depth Stencil state 


	private:
				// Member variables
		ID3D11SamplerState* m_PointSampler; // Variable for the Point sampler
		ID3D11SamplerState* m_TrilinearSampler; // Variable for the Trilinear sampler
		ID3D11SamplerState* m_Anisotropic4xSampler; // Variable for the Anisotropic4x sampler
		ID3D11SamplerState* m_BilinearMirrorSampler; // Variable for the Anisotropic4x sampler

		ID3D11BlendState* m_NoBlendingState;  // Variable for the No Blending state
		ID3D11BlendState* m_AdditiveBlendingState; // Variable for the Additive Blending state
		ID3D11BlendState* m_AlphaBlendingState; // Variable for the Additive Blending state
		ID3D11BlendState* m_MultiplicativeBlendingState; // Variable for the Additive Blending state

		ID3D11RasterizerState* m_CullBackState; // Variable for the Cull Back state
		ID3D11RasterizerState* m_CullFrontState; // Variable for the Cull Front state
		ID3D11RasterizerState* m_CullNoneState; // Variable for the Cull None state
		ID3D11RasterizerState* m_WireframeState; // Variable for the Cull None state

		ID3D11DepthStencilState* m_UseDepthBufferState; // Variable for the Use Depth Buffer state
		ID3D11DepthStencilState* m_DepthReadOnlyState; // Variable for the Depth Read Only state
		ID3D11DepthStencilState* m_NoDepthBufferState; // Variable for the No Depth Buffer state


	};
}
