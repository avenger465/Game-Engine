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

	ERenderingAPI ParseApplicationSettings::GetRenderingAPI(std::string type)
	{
		if (type == "DirectX11")
		{
			return ERenderingAPI::DirectX11;
		}
		else
		{
			return ERenderingAPI::None;
		}
		
	}
}
