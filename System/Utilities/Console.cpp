

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "src/thirdParty/ImGUI/imgui.h"
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#include "Console.h"
#endif

#include <string>

std::vector<std::pair<Console::Type, std::string>> Console::mesQue(50);
unsigned int Console::qIndex = 0;
	

Console::Console()
{
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
	Commands.push_back("HELP");
	Commands.push_back("HISTORY");
	Commands.push_back("CLEAR");
	Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
	AutoScroll = true;
	ScrollToBottom = false;
}
    

    Console::~Console()
    {
		mesQue.clear();
    	for (int i = 0; i < History.Size; i++)
    	free(History[i]);
    }
    


       void Console::Draw(const char * title, bool * p_open)
       {
    	   ImGui::SetNextWindowSize(ImVec2(520, 100), ImGuiCond_FirstUseEver);
    	   if (!ImGui::Begin(title, p_open))
    	   {
    		   ImGui::End();
    		   return;
    	   }
    
    	   
    	   if (ImGui::BeginPopupContextItem())
    	   {
    		   if (ImGui::MenuItem("Close Console"))
    			   *p_open = false;
    		   ImGui::EndPopup();
    	   }
			

    	   //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }
    
    	   ImGui::Separator();
    
    	   // Options menu
    	   if (ImGui::BeginPopup("Options"))
    	   {
    		   ImGui::Checkbox("Auto-scroll", &AutoScroll);
    		   ImGui::EndPopup();
    	   }
    
    	   // Options, Filter
    	   if (ImGui::Button("Options"))
    		   ImGui::OpenPopup("Options");
    	   ImGui::SameLine();
    	   Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    	   ImGui::Separator();
    
    	   const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text

    	   ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
    	   
    
    	 


    	   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

    	   
		   for (auto mes : mesQue)
		   {
			   if (!Filter.PassFilter(mes.second.c_str()))
				    continue;

			   bool pop_color = false;
			   switch (mes.first)
			   {
			   case Console::Type::Error: { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; break; }
			   case Console::Type::Info:  { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 0.2f, 1.0f)); pop_color = true; break; }
			   default: break;
			   }
			   
			   //else if (strncmp(item, "# ", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
			   if(mes.second != "")
			    ImGui::TextUnformatted(mes.second.c_str());
			   if (pop_color)
				   ImGui::PopStyleColor();

		   }
		   
    	   
    	   if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
    		   ImGui::SetScrollHereY(1.0f);
    	   ScrollToBottom = false;
    
    	   ImGui::PopStyleVar();
    	   ImGui::EndChild();
    	   ImGui::Separator();
    
    	   // Command-line
    	   bool reclaim_focus = false;
    	   if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
    	   {
    		   char* s = InputBuf;
    		   Strtrim(s);
    		   if (s[0])
    			   ExecCommand(s);
    		   strcpy(s, "");
    		   reclaim_focus = true;
    	   }
    
    	   // Auto-focus on window apparition
    	   ImGui::SetItemDefaultFocus();
    	   if (reclaim_focus)
    		   ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    
    	   ImGui::End();
       }
    

    	 int Console::TextEditCallback(ImGuiInputTextCallbackData * data)
    	 {
    		
    		switch (data->EventFlag)
    		{
    		
    		
    		case ImGuiInputTextFlags_CallbackHistory:
    		{
    			// Example of HISTORY
    			const int prev_history_pos = HistoryPos;
    			if (data->EventKey == ImGuiKey_UpArrow)
    			{
    				if (HistoryPos == -1)
    					HistoryPos = History.Size - 1;
    				else if (HistoryPos > 0)
    					HistoryPos--;
    			}
    			else if (data->EventKey == ImGuiKey_DownArrow)
    			{
    				if (HistoryPos != -1)
    					if (++HistoryPos >= History.Size)
    						HistoryPos = -1;
    			}
    
    			// A better implementation would preserve the data on the current input line along with cursor position.
    			if (prev_history_pos != HistoryPos)
    			{
    				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
    				data->DeleteChars(0, data->BufTextLen);
    				data->InsertChars(0, history_str);
    			}
    		}
    		}
    		return 0;
     	 }

		 void Console::PrintError(const std::string & message)
		 {
			 Console::mesQue.at(qIndex) = { Console::Type::Error, std::move(message) } ;
			 qIndex++;
			 if (qIndex == 50) qIndex = 0;
			 
		 }

		 void Console::PrintInfo(const std::string & message)
		 {
			 Console::mesQue.at(qIndex) = { Console::Type::Info, std::move(message) };
			 qIndex++;
			 if (qIndex == 50) qIndex = 0;
		 }

		 void Console::Print(const std::string & message)
		 {
			 Console::mesQue.at(qIndex) = { Console::Type::Text, std::move(message) };
			 qIndex++;
			 if (qIndex == 50) qIndex = 0;
		 }

		 
    
		 void Console::ExecCommand(const char * command_line)
		 {
			 Print(command_line);

			 // Process command
			 if (Stricmp(command_line, "CLEAR") == 0)
			 {
				 
			 }
			 if (std::strstr(command_line, "SetPosition("))
			 {
				 std::string line = command_line;
				 line.erase(0, line.find("(") + 1);
				 float number = std::stof(line.substr(0, line.find(",")));
				 line.erase(0, line.find(",") + 1);
				 float number2 = std::stof(line.substr(0, line.find(",")));
				 float number3 = std::stof(line.substr(line.find(",") + 1, line.find(")")));

				 PrintError(std::to_string(number));
				 PrintError(std::to_string(number2));
				 PrintError(std::to_string(number3));
			 }
			 else if (Stricmp(command_line, "HELP") == 0)
			 {
				 PrintInfo("Commands:");
				 int i = 1;
				 for (auto& s : Commands)
				 {
					 Print(i + ". " + s);
				 }
			 }
			 else
			 {
				 using namespace std::literals::string_literals;
				 PrintInfo("Unknown command:"s + command_line);
			 }

			 
		 }