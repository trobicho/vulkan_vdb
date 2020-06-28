/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dev_gui.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/15 12:55:30 by trobicho          #+#    #+#             */
/*   Updated: 2020/06/19 06:24:48 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dev_gui.h"

VkResult				err;

Dev_gui::Dev_gui(My_vulkan &vulk, GLFWwindow *win): m_vulk(vulk)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(win, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	vulk.init_imgui(init_info);

	m_framebuffer.resize(init_info.ImageCount);
	create_desc_pool(init_info);
	init_info.DescriptorPool = m_desc_pool;
	create_render_pass(init_info);
	create_framebuffer(init_info);

	m_command_buffer.resize(init_info.ImageCount);
	m_vulk.create_command_pool(&m_command_pool
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	m_vulk.create_command_buffer(m_command_buffer.data()
			, static_cast<uint32_t>(m_command_buffer.size()), m_command_pool);

	ImGui_ImplVulkan_Init(&init_info, m_render_pass);
	for (int i = 0; i < init_info.ImageCount; i++)
	{
		My_vulkan::check_vk_result(
				(VkResult)vulk.begin_single_time_command(m_command_buffer[i]));
		ImGui_ImplVulkan_CreateFontsTexture(m_command_buffer[i]);
		vulk.end_single_time_command(m_command_buffer[i]);
	}
}

void	Dev_gui::new_frame()
{
	uint32_t					img_index = m_vulk.get_img_index();
	VkCommandBufferBeginInfo	cmd_info = {};
	VkRenderPassBeginInfo		rp_info = {};
	VkClearValue				clear_value = {};

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();

    My_vulkan::check_vk_result(vkResetCommandPool(m_vulk.get_device_ref()
			, m_command_pool, 0));
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VkResult err = vkBeginCommandBuffer(m_command_buffer[img_index], &cmd_info);

    My_vulkan::check_vk_result(err);
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.renderPass = m_render_pass;
    rp_info.framebuffer = m_framebuffer[img_index];
    rp_info.renderArea.extent = m_vulk.get_swapchain_extent();
    rp_info.clearValueCount = 1;
	clear_value.color = {0.1f, 0.1f, 0.8f, 1.0f};
    rp_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(m_command_buffer[img_index]
			, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData()
		, m_command_buffer[img_index]);
	vkCmdEndRenderPass(m_command_buffer[img_index]);
	My_vulkan::check_vk_result(vkEndCommandBuffer(m_command_buffer[img_index]));

}

void	Dev_gui::create_framebuffer(ImGui_ImplVulkan_InitInfo &init_info)
{
	VkImageView				attachment;
    VkFramebufferCreateInfo	info = {};
	VkExtent2D				extent = m_vulk.get_swapchain_extent();

    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = m_render_pass;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;
	auto& img_view = m_vulk.get_img_view_ref();
    for (uint32_t i = 0; i < init_info.ImageCount; i++)
    {
        attachment = img_view[i];
        err = vkCreateFramebuffer(init_info.Device, &info
			, nullptr, &m_framebuffer[i]);
        My_vulkan::check_vk_result(err);
    }
}

void	Dev_gui::create_desc_pool(ImGui_ImplVulkan_InitInfo &init_info)
{
	VkDescriptorPoolCreateInfo	pool_info = {};
	VkDescriptorPoolSize		pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	err = vkCreateDescriptorPool(init_info.Device
			, &pool_info, nullptr, &m_desc_pool);
	My_vulkan::check_vk_result(err);
}

void	Dev_gui::create_render_pass(ImGui_ImplVulkan_InitInfo &init_info)
{
	VkAttachmentDescription		attachment = {};
	VkAttachmentReference		color_attachment = {};
	VkSubpassDescription		subpass = {};
	VkSubpassDependency			dependency = {};
	VkRenderPassCreateInfo		render_pass_info = {};

	attachment.format = m_vulk.get_swap_chain_img_format();
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment.attachment = 0;
	color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	My_vulkan::check_vk_result(vkCreateRenderPass(init_info.Device
			, &render_pass_info, nullptr, &m_render_pass));
}
