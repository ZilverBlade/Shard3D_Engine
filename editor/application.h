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
	class EditorApplication {
	public:
		EditorApplication();
		~EditorApplication();

		EditorApplication(const EditorApplication&) = delete;
		EditorApplication& operator=(const EditorApplication&) = delete;

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

		// Engine components
		EngineWindow engineWindow{ 1280, 720, "Shard3D Engine" };
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };
		
		FrameBufferAttachment* mainColorFramebufferAttachment;
		FrameBufferAttachment* mainDepthFramebufferAttachment;

		FrameBufferAttachment* mainResolveFramebufferAttachment;
		FrameBufferAttachment* mainDepthResolveFramebufferAttachment;
		FrameBuffer* mainFrameBuffer;
		RenderPass* mainRenderpass;

		FrameBufferAttachment* ppoColorFramebufferAttachment;
		FrameBuffer* ppoFrameBuffer;
		RenderPass* ppoRenderpass;

		// ECS
		sPtr<ECS::Level> level;
	};
}