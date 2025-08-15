import bpy
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
            self.report({'ERROR'}, f"Unknown tag: {self.tag}")
            return {'CANCELLED'}
        
        # 初期配置位置（デフォルト）
        location = (0, 0, 1)
        # 初期スケール（デフォルト）
        x_scale = 1.0
        y_scale = 1.0
        z_scale = 1.0

        # PLAYERタグの場合はZ=2に調整
        if self.tag == "PLAYER":
            location = (0, 0, 2)
            z_scale = 2.0
        # NORMAL_ENEMYタグの場合はZ=2に調整
        elif self.tag == "NORMAL_ENEMY":
            location = (0, 0, 2)
            z_scale = 2.0
        # IMMOBILE_ENEMYタグの場合はZ=2に調整
        elif self.tag == "IMMOBILE_ENEMY":
            location = (0, 0, 2)
            z_scale = 2.0
        # BOSS_ENEMYタグの場合はZ=3に調整
        elif self.tag == "BOSS_ENEMY":
            location = (0, 0, 3)
            x_scale = 5.0
            y_scale = 5.0
            z_scale = 3.0
        # WATPOINTタグの場合はSphereを生成
        elif self.tag == "WAYPOINT":
            location = (0, 0, 1)
            bpy.ops.mesh.primitive_uv_sphere_add(radius=0.5, location=location)
            obj = context.active_object
            obj.name = f"{info['name']}"
            obj["object_tag"] = self.tag # カスタムプロパティとして保存
            mat = create_material(self.tag)
            if mat:
                if obj.data.materials:
                    obj.data.materials[0] = mat
                else:
                    obj.data.materials.append(mat)
            return {'FINISHED'}

        # キューブ生成
        bpy.ops.mesh.primitive_cube_add(size=2, location=location)
        obj = context.active_object
        obj.name = f"{info['name']}"
        obj["object_tag"] = self.tag # カスタムプロパティとして保存
        obj.scale = (x_scale, y_scale, z_scale) # スケール調整

        # マテリアル設定
        mat = create_material(self.tag)
        if mat:
            if obj.data.materials:
                obj.data.materials[0] = mat
            else:
                obj.data.materials.append(mat)
            
            return {'FINISHED'}