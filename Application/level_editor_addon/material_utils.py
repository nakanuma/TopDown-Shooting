import bpy
from .contants import TAG_INFO

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