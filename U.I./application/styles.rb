# frozen_string_literal: true

def vie_styles
  action_height = 39
  toolbox_with = 52
  inspector_with = 120
  navigation_height = 30
  margin = 9
  smooth = 3
  # front_color= {red: 0.14, green: 0.13, blue: 0.12}
  {
    margin: margin,
    toolbox_with: 60,
    module_label: { left: 3, top: 33, visual: { size: 9 }, attached: [:active_color] },
    inspector_with: inspector_with,
    navigation_height: 30,
    text_color: { red: 0.6, green: 0.6, blue: 0.6, id: :text_color },
    list_style: { colors: %i[gray white], width: :auto, left: 0, right: 0, height: 15,
                  items: { color: :orange, size: 36, align: :left, margin: 3 } },
    back_color: { red: 0.1, green: 0.1, blue: 0.1, id: :back_color },
    title: { id: :title, visual: { size: 21 }, width: :auto, left: 25, center: :horizontal, top: 9,
             attached: [:inactive_color] },
    toolbox_color: { red: 0.10, green: 0.10, blue: 0.10, id: :toolbox_color },
    center_color: { red: 0, green: 0, blue: 0, alpha: 0, id: :center_color },
    action_color: { red: 0, green: 0, blue: 0, alpha: 0, id: :action_color },
    module_color: { red: 0.7, green: 0.3, blue: 0, alpha: 1, id: :module_color },
    inspector_color: { red: 0.09, green: 0.09, blue: 0.09, id: :inspector_color },
    active_color: { red: 0.8, green: 0.8, blue: 0.8, id: :active_color },
    select_color: { red: 0.8, green: 0, blue: 0.3, id: :select_color },
    inactive_color: { red: 0.3, green: 0.3, blue: 0.3, id: :inactive_color },
    dark_color: { red: 0.127, green: 0.127, blue: 0.127, id: :dark_color },
    invisible_color: { red: 0, green: 0, blue: 0, alpha: 0, id: :invisible_color },
    darker_color: { red: 0.127, green: 0.127, blue: 0.127, id: :darker_color },
    cell_color: { red: 0.17, green: 0.15, blue: 0.13, id: :cell_color },
    cell_over: { red: 0.23, green: 0.27, blue: 0.15, id: :cell_over },
    cell_selected: { red: 1, green: 1, blue: 0, id: :cell_selected },
    cell_connected: { red: 154 / 255, green: 205 / 255, blue: 50 / 255, id: :cell_connected },
    cell_shadow: { blur: 9, left: 3, top: 3, id: :cell_shadow, red: 0, green: 0, blue: 0, alpha: 0.3,
                   direction: :inset },
    invert_shadow: { blur: 9, left: 3, top: 3, id: :invert_shadow, red: 0, green: 0, blue: 0, alpha: 0.3,
                     direction: :inset },
    toolbox_style: { id: :toolbox, overflow: :hidden, left: margin, top: action_height + margin, smooth: smooth,
                     width: toolbox_with, bottom: 30, height: 420, attached: [:toolbox_color] },
    inspector_style: { id: :inspector, left: toolbox_with + margin * 2, top: action_height + margin, smooth: smooth,
                       width: inspector_with, bottom: 0, height: 420, overflow: :auto, attached: %i[invert_shadow inspector_color] },
    action_style: { id: :action, left: toolbox_with, top: 0, width: :auto, right: 0, height: action_height,
                    attached: [:action_color] },
    filer: { id: :filer, top: 12, bottom: 36, left: 6, width: inspector_with - 12, overflow: :auto, height: :auto,
             attached: [:darker_color], smooth: 6 },
    support_style: { left: 6, width: 23, height: 23, attached: [:invisible_color] },
    module_style: { left: 6, width: 23, height: 23, attached: [:module_color] },
    center_style: { id: :center, left: toolbox_with + inspector_with + margin * 3, bottom: navigation_height,
                    top: action_height, right: 0, width: :auto, height: :auto, attached: [:center_color] },
    matrix_style: { top: 0, left: 0, smooth: 0, columns: { count: 8 },
                    rows: { count: 8 },
                    width: 440,
                    height: 440,
                    color: { alpha: 0 },
    },
    active_state: {attached: :active_color},
    default_state: {detached: [:select_color,:active_color]},
    select_state: {attached: :active_color}

  }
end

# colors

# this the color for selected cells
color(vie_styles[:cell_selected])
# this the color for overed cells
color(vie_styles[:cell_over])
color(vie_styles[:back_color])
color(vie_styles[:toolbox_color])
color(vie_styles[:action_color])
color(vie_styles[:inspector_color])
color(vie_styles[:center_color])
color(vie_styles[:darker_color])
color(vie_styles[:dark_color])
color(vie_styles[:text_color])
color(vie_styles[:active_color])
color(vie_styles[:inactive_color])
color(vie_styles[:cell_connected])
color(vie_styles[:invisible_color])
color(vie_styles[:module_color])
shadow(vie_styles[:cell_shadow])
shadow(vie_styles[:invert_shadow])
color(vie_styles[:select_color])
