# ブレンダーに登録するアドオン情報
bl_info = {
    "name": "レベルエディタ_個人制作用",
    "author": "Nakanuma",
    "version": (1, 0),
    "blender": (4, 4, 0),
    "location": "",
    "description": "レベルエディタ_個人制作用",
    "warning": "",
    "support": "TESTING",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}

import bpy
from .add_object_operator import OBJECT_OT_add_tagged_object
from .export_json_operator import OBJECT_OT_export_tagged_objects
from .import_json_operator import OBJECT_OT_import_tagged_objects
from .tagged_object_panel import OBJECT_PT_tagged_object_panel

# ---------------------------
# 登録・登録解除
# ---------------------------
classes = (
    OBJECT_OT_add_tagged_object,
    OBJECT_OT_export_tagged_objects,
    OBJECT_OT_import_tagged_objects,
    OBJECT_PT_tagged_object_panel
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    print("エディタを有効化しました")

def unregister():
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    print("エディタを無効化しました")

if __name__ == "__main__":
    register()