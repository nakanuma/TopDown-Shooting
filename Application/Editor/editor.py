import bpy
import json
import os

# タグと対応する情報を定義
TAG_INFO = {
    "PLAYER":{
        "name": "Player",
        "color": (0.0, 0.5, 1.0, 1.0), # 水色
    },
    "NORMAL_ENEMY": {
        "name": "Noraml_Enemy",
        "color": (1.0, 0.5, 0.0, 1.0), # オレンジ色
    },
    "NORMAL_OBSTACLE": {
        "name": "Normal_Obstacle",
        "color": (0.5, 0.5, 0.5, 1.0), # 灰色
    }
}

# ---------------------------
# マテリアル生成ユーティリティ
# ---------------------------
def create_material(tag):
    info = TAG_INFO[tag]
    name = f"Mat_{tag}"
    mat = bpy.data.materials.get(name)
    if not mat:
        mat = bpy.data.materials.new(name=name)
        mat.use_nodes = True
    
    # ノードツリー構築
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links

    # 全ノード削除して再構築
    for node in nodes:
        nodes.remove(node)
    
    # 新しく Principled BSDF ノードと出力ノードを作成
    output_node = nodes.new(type='ShaderNodeOutputMaterial')
    bsdf_node = nodes.new(type='ShaderNodeBsdfPrincipled')

    # 色設定
    bsdf_node.inputs["Base Color"].default_value = info["color"]

    # ノード接続
    links.new(bsdf_node.outputs["BSDF"], output_node.inputs["Surface"])

    return mat

# ---------------------------
# オブジェクト配置オペレーター
# ---------------------------
class OBJECT_OT_add_tagged_object(bpy.types.Operator):
    bl_idname = "object.add_tagged_object"
    bl_label = "Add Tagged Object"
    bl_options = {'REGISTER', 'UNDO'}

    tag: bpy.props.StringProperty()

    def execute(self, context):
        info = TAG_INFO.get(self.tag)
        if not info:
            self.report({'ERROR'}, f"Unknown tag: {self.tag}")
            return {'CANCELLED'}
        
        # キューブ生成
        bpy.ops.mesh.primitive_cube_add(size=2, location=(0, 0, 1))
        obj = context.active_object
        obj.name = f"{info['name']}"
        obj["object_tag"] = self.tag # カスタムプロパティとして保存

        # マテリアル設定
        mat = create_material(self.tag)
        if mat:
            if obj.data.materials:
                obj.data.materials[0] = mat
            else:
                obj.data.materials.append(mat)
            
            return {'FINISHED'}

# ---------------------------
# JSON出力オペレーター
# ---------------------------
class OBJECT_OT_export_tagged_objects(bpy.types.Operator):
    bl_idname = "object.export_tagged_objects_json"
    bl_label = "Export Tagged Objects as JSON"

    filepath: bpy.props.StringProperty(subtype="FILE_PATH")

    def execute(self, context):
        export_data = []

        for obj in bpy.context.scene.objects:
            if "object_tag" in obj:
                data = {
                    "tag": obj["object_tag"],
                    "location": list(obj.location),
                    "rotation": list(obj.rotation_euler),
                    "scale": list(obj.scale),
                }
                export_data.append(data)
        
        try:
            with open(self.filepath, 'w', encoding='utf-8') as f:
                json.dump(export_data, f, indent=4)
            self.report({'INFO'}, f"Exported {len(export_data)} objects to {self.filepath}")
            return {'FINISHED'}
        except Exception as e:
            self.report({'ERROR'}, f"Failed to export: {e}")
            return {'CANCELLED'}
        
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

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

        for tag, info in TAG_INFO.items():
            op = layout.operator("object.add_tagged_object", text=f"Add {info['name']}")
            op.tag = tag

        layout.separator()
        layout.operator("object.export_tagged_objects_json", text="Export JSON")

# ---------------------------
# 登録・登録解除
# ---------------------------
classes = (
    OBJECT_OT_add_tagged_object,
    OBJECT_OT_export_tagged_objects,
    OBJECT_PT_tagged_object_panel
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

def unregister():
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()