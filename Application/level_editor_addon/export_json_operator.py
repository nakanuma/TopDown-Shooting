import bpy
import json
import math

# ---------------------------
# JSON出力オペレーター
# ---------------------------
class OBJECT_OT_export_tagged_objects(bpy.types.Operator):
    bl_idname = "object.export_tagged_objects_json"
    bl_description = "Export tagged objects to a JSON file"
    bl_label = "Export"

    # ファイルパス選択用のプロパティ定義
    filepath: bpy.props.StringProperty(
        name="File Path",
        description="File path to export the JSON data",
        default="",
        subtype="FILE_PATH"
        )

    def execute(self, context):
        # JSONに書き出すデータを格納するリスト
        export_data = []
        scene = context.scene

        # ステージ設定データを最初に追加
        stage_config = {
            "tag": "STAGE_CONFIG",
            "stage_type": scene.stage_type_enum
        }
        export_data.append(stage_config)

        # シーン内全てのオブジェクトをループ
        for obj in bpy.context.scene.objects:
            if "object_tag" in obj:
                # オブジェクトの情報をまとめてリストに追加
                data = {
                    "tag": obj["object_tag"],
                    "location": list(obj.location),
                    "rotation": [math.degrees(angle) for angle in obj.rotation_euler], #degreeに変換
                    "scale": list(obj.scale),
                    "is_collider": obj.get("is_collider", 1)
                }

                if "colliderSize" in obj:
                    data["colliderSize"] = list(obj["colliderSize"])

                # テレポーター用
                if "pair_id" in obj:
                    data["pair_id"] = obj.get("pair_id")

                export_data.append(data)
        
        # 選択されたファイルパスにJSONデータとして保存
        try:
            with open(self.filepath, 'w', encoding='utf-8') as f:
                json.dump(export_data, f, indent=4)
            self.report({'INFO'}, f"Exported {len(export_data)} objects to {self.filepath}")
            return {'FINISHED'}
        # 失敗したらエラーを表示
        except Exception as e:
            self.report({'ERROR'}, f"Failed to export: {e}")
            return {'CANCELLED'}
        
    # ファイル選択ダイアログ
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}