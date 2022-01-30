#include "sdlggerat.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"
#include "clibretro.h"
#include <sstream>

ImGuiFileDialog romloader;
const char *ss_filters = "Savestates (*.state){.state}";
const char *filters = "Source files (*.cpp *.h *.hpp){.cpp,.h,.hpp},Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg},.md";
static ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_Default;

enum
{
  digi_pad,
  ana_pad,
  mousie
};
int controller_type = digi_pad;

static bool coresettings = false;
const char *checkbox_allowable[] = {"enabled|disabled", "disabled|enabled", "True|False", "False|True", "On|Off", "Off|On"};
const char *true_vals[] = {"enabled", "true", "on"};

void sdlggerat_menu()
{
  CLibretro *instance = CLibretro::get_classinstance();
  instance->core_run();
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Load ROM/ISO"))
        romloader.OpenModal("ChooseFileDlgKey", " Choose a ROM/ISO", filters, ".", "", 1, nullptr, flags);

      ImGui::Separator();

      if (ImGui::MenuItem("Load Savestate"))
        romloader.OpenModal("LoadSaveState", "Load a savestate", ss_filters, ".", "", 1, nullptr, flags);

      if (ImGui::MenuItem("Save Savestate"))
        romloader.OpenModal("SaveSaveState", "Save a savestate", ss_filters, ".", "", 1, IGFDUserDatas("SaveFile"), ImGuiFileDialogFlags_ConfirmOverwrite);

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Options"))
    {
      if (ImGui::MenuItem("Core Settings..."))
        coresettings = true;

      ImGui::Separator();
      if (ImGui::BeginMenu("Input device"))
      {
        if (ImGui::BeginMenu("Joypad"))
        {
          if (ImGui::MenuItem("Analog", nullptr, controller_type == ana_pad))
            controller_type = ana_pad;
          if (ImGui::MenuItem("Digital", nullptr, controller_type == digi_pad))
            controller_type = digi_pad;
          ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Mouse", nullptr, controller_type == mousie))
          controller_type = mousie;
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (romloader.Display("ChooseFileDlgKey"))
  {
    // action if OK
    if (romloader.IsOk())
    {
      std::string filePathName = romloader.GetFilePathName();
      std::string filePath = romloader.GetCurrentPath();
      // action
    }
    // close
    romloader.Close();
  }

  if (romloader.Display("LoadSaveState"))
  {
    // action if OK
    if (romloader.IsOk())
    {
      std::string filePathName = romloader.GetFilePathName();
      std::string filePath = romloader.GetCurrentPath();
      // action
    }
    // close
    romloader.Close();
  }

  if (romloader.Display("SaveSaveState"))
  {
    // action if OK
    if (romloader.IsOk())
    {
      std::string filePathName = romloader.GetFilePathName();
      std::string filePath = romloader.GetCurrentPath();
      // action
    }
    // close
    romloader.Close();
  }

  if (coresettings)
  {
    ImGui::PushItemWidth(200);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Core Settings"))
    {
      for (int i = 0; i < instance->core_variables.size(); i++)
      {
        if (instance->core_variables[i].config_visible)
        {
          std::string descript = instance->core_variables[i].description;
          std::string usedv = instance->core_variables[i].usevars;
          std::string var = instance->core_variables[i].var;

          int sel_idx = instance->core_variables[i].sel_idx;
          std::string current_item = instance->core_variables[i].config_vals[sel_idx];
          bool checkbox_made = false;
          bool checkbox_enabled = false;
          for (int j = 0; j < IM_ARRAYSIZE(checkbox_allowable); j++)
          {
            if (checkbox_enabled)
              break;
            if (stricmp(usedv.c_str(), checkbox_allowable[j]) == 0)
            {
              checkbox_made = true;
              for (int k = 0; k < IM_ARRAYSIZE(true_vals); k++)
                if (stricmp(var.c_str(), true_vals[k]) == 0)
                  checkbox_enabled = true;
            }
          }

          std::string hidden = "##" + descript;
          if (checkbox_made)
          {
            
            int total_w = descript.length(); ImGui::Text(descript.c_str()); ImGui::SameLine(450); ImGui::SetNextItemWidth(total_w); 
            if (ImGui::Checkbox(hidden.c_str(), &checkbox_enabled))
            {
              instance->core_variables[i].sel_idx ^= 1;
              std::string change = instance->core_variables[i].config_vals[instance->core_variables[i].sel_idx];
              instance->core_variables[i].var = change;
              instance->variables_changed = true;
            }
              
          }
          else
          {
             int total_w = 200; ImGui::Text(descript.c_str()); ImGui::SameLine(650-200); ImGui::SetNextItemWidth(total_w); 
            if (ImGui::BeginCombo(hidden.c_str(), current_item.c_str())) // The second parameter is the label previewed before opening the combo.
            {
              for (int n = 0; n < instance->core_variables[i].config_vals.size(); n++)
              {
                bool is_selected = (instance->core_variables[i].sel_idx == n); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(instance->core_variables[i].config_vals[n].c_str(), is_selected))
                {
                  instance->core_variables[i].sel_idx = n;
                }
                  

                if (is_selected)
                  ImGui::SetItemDefaultFocus();
              }
              ImGui::EndCombo();
            }
          }
        }
      }

      // click ok when finished adjusting
      if (ImGui::Button("OK"))
        coresettings = false;

      ImGui::End();
    }
  }
}