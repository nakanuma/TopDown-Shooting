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

        # EVENT_TRIGGER用のモデル設定
        if self.tag ==  "EVENTTRIGGER":
            # 見た目をワイヤーフレームにする
            obj.display_type = 'WIRE'

        mat = create_material(self.tag)
        if mat:
            if obj.data.materials:
                obj.data.materials[0] = mat
            else:
                obj.data.materials.append(mat)
        
        return {'FINISHED'}
    
# ---------------------------
# コライダー一括設定オペレーター
# ---------------------------
class OBJECT_OT_set_collider_property(bpy.types.Operator):
    bl_idname = "object.set_collider_property"
    bl_label = "Set Collider Property"
    bl_description = "選択中のオブジェクトのコライダー有効/無効を一括設定します"
    bl_options = {'REGISTER', 'UNDO'}

    is_collider: bpy.props.BoolProperty(name="Is Collider")

    def execute(self, context):
        count = 0
        for obj in context.selected_objects:
            if obj.type == 'MESH':
                obj["is_collider"] = 1 if self.is_collider else 0
                count += 1
            
        status = "ON" if self.is_collider else "OFF"
        self.report({'INFO'}, f"{count}個のオブジェクトのコライダーを{status}にしました")
        return {'FINISHED'}
    
# ---------------------------
# コライダー無効オブジェクトをワイヤーフレーム表示するオペレーター
# ---------------------------
class OBJECT_OT_visualize_colliders(bpy.types.Operator):
    bl_idname = "object.visualize_colliders"
    bl_label = "Visualize Colliders"
    bl_description = "コライダー設定に応じて表示形式を切り替えます"

    # 状態保持用
    show_visualization : bpy.props.BoolProperty(default=False)

    def execute(self, context):
        for obj in bpy.data.objects:
            if obj.type != 'MESH':
                continue

            is_collider = obj.get("is_collider", 1)

            if self.show_visualization:
                # コライダー無しをワイヤーフレームにする
                if is_collider == 0:
                    obj.display_type = 'WIRE'
                    obj.show_wire = True
                else:
                    obj.display_type = 'TEXTURED'
                    obj.show_wire = False
            else:
                # 通常モードに戻す
                obj.display_type = 'TEXTURED'
                obj.show_wire = False

        # 次回実行時のために状態を反転
        self.show_visualization = not self.show_visualization
        return {'FINISHED'}
