import bpy
from .contants import TAG_INFO

# ---------------------------
# UIパネル
# ---------------------------
class OBJECT_PT_tagged_object_panel(bpy.types.Panel):
    bl_label = "Tagged Object Generator"
    bl_idname = "OBJECT_PT_tagged_object_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "MapEditor"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Add Tagged Objects:")

        # 区分ごとにセパレーターを挿入
        last_group = None
        for tag, display_name in [(key, TAG_INFO[key]["name"]) for key in TAG_INFO]:
            if tag in ["PLAYER", "CONTAINER", "TELEPORTER", "POWERGENERATOR", "WAYPOINT_BOTTOMLEFT"]:
                layout.separator()

            op = layout.operator("object.add_tagged_object", text=f"Add {display_name}")
            op.tag = tag

        layout.separator()
        layout.operator("object.import_tagged_objects_json", text="Import JSON")
        layout.operator("object.export_tagged_objects_json", text="Export JSON")