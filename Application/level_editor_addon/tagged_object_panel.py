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
        scene = context.scene
        
        # ステージ全体設定
        box = layout.box()
        box.label(text="Global Stage Settings");
        # シーンに登録したEnumPropertyを表示
        box.prop(scene, "stage_type_enum", expand=True)

        layout.separator()
        layout.label(text="Add Tagged Objects:")

        # 区分ごとにセパレーターを挿入
        last_group = None
        for tag, display_name in [(key, TAG_INFO[key]["name"]) for key in TAG_INFO]:
            if tag in ["PLAYER", "CONTAINER", "TELEPORTER", "POWERGENERATOR", "WAYPOINT_BOTTOMLEFT"]:
                layout.separator()

            op = layout.operator("object.add_tagged_object", text=f"Add {display_name}")
            op.tag = tag

        layout.separator()

        # コライダー一括設定
        col = layout.column(align=True)
        col.label(text="Collider Settings (Selected):")

        row = col.row(align=True)
        # コライダーONボタン
        op_on = row.operator("object.set_collider_property", text="Collider ON")
        op_on.is_collider = True

        # コライダーOFFボタン
        op_off = row.operator("object.set_collider_property", text='OFF')
        op_off.is_collider = False

        # 現在の選択物の状態を表示
        if context.active_object and "is_collider" in context.active_object:
            val = context.active_object["is_collider"]
            state = "ON" if val == 1 else "OFF"
            col.label(text=f"Current Active: {state}")

        layout.separator()
        col = layout.column(align=True)
        col.label(text="Debug View")
        # 可視化ボタン
        op = col.operator("object.visualize_colliders", text="Highlight Non-Colliders")

        layout.separator()
        layout.operator("object.import_tagged_objects_json", text="Import JSON")
        layout.operator("object.export_tagged_objects_json", text="Export JSON")