#include "epch.h"
#include "ParseApplicationSettings.h"

namespace Engine
{
	WindowProperties ParseApplicationSettings::ReadWindowSettings(std::string filename)
	{
		tinyxml2::XMLDocument ApplicationSettingsDocument;

		tinyxml2::XMLError error = ApplicationSettingsDocument.LoadFile(filename.c_str());
		if(error != tinyxml2::XML_SUCCESS) return WindowProperties();

		tinyxml2::XMLElement* element = ApplicationSettingsDocument.FirstChildElement();
		if (element == nullptr) return WindowProperties();

		while (element != nullptr)
		{
			std::string elementName = element->Name();
			if (elementName == "WindowSettings")
			{
				tinyxml2::XMLElement* childElement = element->FirstChildElement("Title");
				const tinyxml2::XMLAttribute* attr = childElement->FindAttribute("Name");
				if (attr != nullptr) m_WindowProps.Title = attr->Value();


				childElement = element->FirstChildElement("Dimensions");
				attr = childElement->FindAttribute("Height");
				if (attr != nullptr) m_WindowProps.Height = attr->FloatValue();
				attr = childElement->FindAttribute("Width");
				if (attr != nullptr) m_WindowProps.Width = attr->FloatValue();

				childElement = element->FirstChildElement("RenderingAPI");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr) m_WindowProps.RenderType = GetRenderingAPI(attr->Value());
			}

			element = element->NextSiblingElement();
		}

		return m_WindowProps;
	}

	void ParseApplicationSettings::SaveWindowSettings(WindowProperties settings)
	{
		std::filesystem::path mainPath = std::filesystem::current_path();
		std::filesystem::path Path = std::filesystem::current_path().parent_path().append("Editor\\data");
		std::filesystem::current_path(Path);

		tinyxml2::XMLDocument doc;

		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.InsertFirstChild(decl);

		tinyxml2::XMLElement* root = doc.NewElement("WindowSettings");
		doc.InsertEndChild(root);
		tinyxml2::XMLElement* child = doc.NewElement("Title");
		child->SetAttribute("Name", settings.Title.c_str());
		root->InsertEndChild(child);

		child = doc.NewElement("Dimensions");
		child->SetAttribute("Height", settings.Height);
		child->SetAttribute("Width", settings.Width);
		root->InsertEndChild(child);

		child = doc.NewElement("RenderingAPI");
		std::string renderType = "";
		if (settings.RenderType == ERenderingAPI::DirectX11)
		{
			renderType = "DirectX11";
		}
		child->SetAttribute("Type", renderType.c_str());
		root->InsertEndChild(child);
		doc.SaveFile("WindowSettings.xml");
		std::filesystem::current_path(mainPath);
	}

	ERenderingAPI ParseApplicationSettings::GetRenderingAPI(std::string type)
	{
		if (type == "DirectX11")
		{
			return ERenderingAPI::DirectX11;
		}
		else if (type == "OpenGL")
		{
			return ERenderingAPI::OpenGL;
		}
		else
		{
			return ERenderingAPI::None;
		}
		
	}
}
