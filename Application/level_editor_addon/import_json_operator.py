import bpy
import json
import math
import os
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

        # JSON読み込み
        try:
            with open(self.filepath, 'r', encoding='utf-8') as f:
                import_data = json.load(f)
        except Exception as e:
            self.report({'ERROR'}, f"Failed to import: {e}")
            return {'CANCELLED'}
        
        addon_directory = os.path.dirname(__file__)

        for item in import_data:
            tag = item.get("tag")
            location = item.get("location", (0, 0, 0))
            rotation = item.get("rotation", (0, 0, 0))
            scale = item.get("scale", (1, 1, 1))

            if tag not in TAG_INFO:
                self.report({'WARNING'}, f"Tag '{tag}' not found in TAG_INFO. Skipping object.")
                continue

            info = TAG_INFO[tag]
            obj = None
            
            # モデル読み込み
            if "model" in info:
                model_path = os.path.join(addon_directory, info["model"])
                if os.path.exists(model_path):
                    bpy.ops.wm.obj_import(
                        'EXEC_DEFAULT',
                        filepath=model_path,
                        forward_axis='Z',
                        up_axis='Y'
                    )
                    obj = context.active_object
                    obj.location = location
                    obj.rotation_euler = [math.radians(angle) for angle in rotation] #radianに変換
                    obj.scale = scale
                    obj.name = info['name']
                    obj["object_tag"] = tag
                    obj["colliderSize"] = info.get("colliderSize", (1.0, 1.0, 1.0))
                    obj["pair_id"] = item.get("pair_id", "EMPTY")
                else:
                    self.report({'WARNING'}, f"Model file '{model_path}' not found. Skipping object.")
                    continue
            
        # インポートしたオブジェクト数の表示
        self.report({'INFO'}, f"Imported{len(import_data)} objects")
        return {'FINISHED'}
    
    # ファイル選択ダイアログ
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}