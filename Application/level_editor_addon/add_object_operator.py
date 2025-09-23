import bpy
import os
from .contants import TAG_INFO
from .material_utils import create_material

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
            self.report({'ERROR'}, f"Tag '{self.tag}' not found in TAG_INFO.")
            return {'CANCELLED'}
        
        # モデルパスが指定されている場合はobjをインポート
        model_path = info.get("model")
        if model_path:
            # アドオンディレクトリからの相対パスをフルパスに変換
            addon_dir = os.path.dirname(__file__)
            full_model_path = os.path.join(addon_dir, model_path)

            # OBJをインポート
            bpy.ops.wm.obj_import(
                'EXEC_DEFAULT',
                filepath=full_model_path,
                forward_axis='Z',
                up_axis='Y'
            )

            obj = context.active_object
            obj.name = info['name']
            obj["object_tag"] = self.tag
            obj["colliderSize"] = info.get("colliderSize", (1.0, 1.0, 1.0))
            obj["pair_id"] = "EMPTY"

            obj.location = info.get("location", (0, 0, 0))

            return {'FINISHED'}
        
        # モデルパスがない場合は基本形状を追加
        bpy.ops.mesh.primitive_cube_add(size=2)
        obj = context.active_object
        obj.name = info['name']
        obj["tag"] = self.tag

        mat = create_material(self.tag)
        if mat:
            if obj.data.materials:
                obj.data.materials[0] = mat
            else:
                obj.data.materials.append(mat)
        
        return {'FINISHED'}