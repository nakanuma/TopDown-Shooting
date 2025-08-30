import bpy
import json
import math
from .contants import TAG_INFO
from .material_utils import create_material

# ---------------------------
# JSON読み込みオペレーター
# ---------------------------
class OBJECT_OT_import_tagged_objects(bpy.types.Operator):
    bl_idname = "object.import_tagged_objects_json"
    bl_description = "Import tagged objects from a JSON file"
    bl_label = "Import"

    # ファイルパス選択用のプロパティ定義
    filepath: bpy.props.StringProperty(
        name="File Path",
        description="File path to import the JSON data",
        default="",
        subtype="FILE_PATH"
        )
    
    def execute(self, context):
        # 全てのオブジェクトを削除
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)

        # 選択されたファイルをJSONとして読み込み
        try:
            with open(self.filepath, 'r', encoding='utf-8') as f:
                import_data = json.load(f)
        # 失敗したらエラーを表示
        except Exception as e:
            self.report({'ERROR'}, f"Failed to read JSON: {e}")
            return {'CANCELLED'}
        
        # 読み込んだ各オブジェクトの配置処理
        for item in import_data:
            # 各項目
            tag = item.get("tag")
            location = item.get("location", [0, 0, 0])
            rotation = item.get("rotation", [0, 0, 0])
            scale = item.get("scale", [1 ,1 ,1])

            # 安全確認のため、存在していないタグがあれば処理スキップ
            if tag not in TAG_INFO:
                self.report({'WARNING'}, f"Unknown tag: {tag}")
                continue

            # 各項目を適用してキューブを生成
            if tag == "WAYPOINT": # Waypointタグの場合はSphereを生成
                bpy.ops.mesh.primitive_uv_sphere_add(radius=0.5, location=location)
            else:
                # キューブを生成
                bpy.ops.mesh.primitive_cube_add(size=2, location=location)
                
            obj = context.active_object
            obj.name = TAG_INFO[tag]["name"]
            obj.rotation_euler = [math.radians(deg) for deg in rotation]
            obj.scale = scale
            obj["object_tag"] = tag

            # テレポーター用のプロパティがあれば設定
            if tag == "TELEPORTER":
                pair_id = item.get("pair_id")
                if pair_id:
                    obj["pair_id"] = pair_id

            # マテリアルの再適用
            mat = create_material(tag)
            if mat:
                if obj.data.materials:
                    obj.data.materials[0] = mat
                else:
                    obj.data.materials.append(mat)
            
        # インポートしたオブジェクト数の表示
        self.report({'INFO'}, f"Imported{len(import_data)} objects")
        return {'FINISHED'}
    
    # ファイル選択ダイアログ
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}