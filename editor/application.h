#pragma once
#define SHARD3D_EDITOR

#include <Shard3D/core.h>
#include <Shard3D/s3dpch.h>
#include <Shard3D/ecs.h>
#include <Shard3D/vulkan_abstr.h>

#include <Shard3D/events/event.h>
#include <Shard3D/events/wnd_event.h>
#include <Shard3D/core/rendering/window.h>
#include <Shard3D/core/rendering/swap_chain.h>
#include <Shard3D/core/rendering/render_pass.h>
#include <Shard3D/core/rendering/renderer.h>

namespace Shard3D {
	class Application {
		static inline constexpr int wndWidth = 1280;
		static inline constexpr int wndHeight = 720;
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();	
	private:
		// Functions
		void setupEngineFeatures();
		void setWindowCallbacks();
		void createRenderPasses();
		void destroyRenderPasses();
		void loadStaticObjects();

		void windowResizeEvent(Events::WindowResizeEvent& e);
		void eventEvent(Events::Event& e);

		void resizeFrameBuffers(uint32_t newWidth, uint32_t newHeight, void* editor, void* ppoSystem);

		// Engine components
		EngineWindow engineWindow{ wndWidth, wndHeight, "Shard3D Engine" };
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };
		
		FrameBufferAttachment* mainColorFramebufferAttachment;
		FrameBufferAttachment* mainDepthFramebufferAttachment;
		FrameBufferAttachment* mainPositionFramebufferAttachment;
		FrameBufferAttachment* mainNormalFramebufferAttachment;
		FrameBufferAttachment* mainMaterialDataFramebufferAttachment;


		FrameBuffer* mainFrameBuffer;
		RenderPass* mainRenderpass;

		FrameBufferAttachment* ppoColorFramebufferAttachment;
		FrameBuffer* ppoFrameBuffer;
		RenderPass* ppoRenderpass;

		// ECS
		sPtr<ECS::Level> level;
	};
}