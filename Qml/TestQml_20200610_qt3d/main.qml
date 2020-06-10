import QtQuick 2.15
import QtQuick.Window 2.15 as QW
import QtQuick.Scene3D 2.15
import Qt3D.Core 2.15
import Qt3D.Render 2.15

//不加别名有冲突
//Qt3D QML的文档不怎么详细，有些可以参见CPP版本的
QW.Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello Qt3D")

    //创建3d场景嵌入到QtQuick
    Scene3D{
        id:scene
        anchors.fill: parent

        //根实体Entity
        Entity {
            //必须将RenderSettings组件设置为场景根实体的组件。
            //它指定渲染策略和选择设置，并托管活动的FrameGraph
            RenderSettings {
                id: renderSettings
                //保存当前活动的FrameGraph
                //Qt 3D渲染方面允许渲染算法完全由数据驱动。
                //该控制的数据结构被称为framegraph
                activeFrameGraph: ClearBuffers {
                    //为什么ColorBuffer渲染不出来三角
                    buffers: ClearBuffers.ColorDepthBuffer
                    clearColor: Qt.rgba(0.0,0.5,0.0,1.0)

                    //可以用来选择Qt3D渲染内容的表面。
                    //该表面可以是窗口表面或屏幕外表面。
                    RenderSurfaceSelector {
                        //与可以在RenderPass上设置的每个材质状态相反，
                        //在RenderStateSet上设置的状态是全局设置的
                        RenderStateSet {
                            renderStates: DepthTest {
                                //如果片段深度小于z缓冲区值，则通过深度测试
                                depthFunction: DepthTest.Less
                            }
                        }
                    }
                }
            }
            //三角实体Entity
            Entity{
                //几何渲染器
                GeometryRenderer {
                    id: geometry
                    //几何体
                    //Geometry类型用于将Attribute对象列表分组在一起，
                    //以形成Qt3D能够使用GeometryRenderer渲染的几何形状。
                    geometry: Geometry {
                        //属性Attribute，对应Shader中的attribute
                        Attribute {
                            id: position
                            attributeType: Attribute.VertexAttribute
                            vertexBaseType: Attribute.Float
                            vertexSize: 3
                            count: 3
                            name: "position"
                            buffer: Buffer {
                                type: Buffer.VertexBuffer
                                usage: Buffer.StaticDraw
                                accessType: Buffer.Write
                                data: new Float32Array(
                                          [
                                              -0.5, -0.5, 0.0,
                                              0.5, -0.5, 0.0,
                                              0.0,  0.5, 0.0,
                                          ])

                            }
                        }
                    }
                }//end GeometryRenderer
                //材质定义如何渲染Entity
                Material {
                    id: material
                    effect: Effect {
                        //一项技术Technique指定一组RenderPass对象，FilterKey对象，Parameter对象和GraphicsApiFilter，
                        //它们共同定义了给定图形API可以渲染的渲染技术。
                        techniques: Technique {
                            //指定使用的图形API过滤器
                            //profile 默认为 NoProfile。 Core模式时，设置为CoreProfile。精简。
                            graphicsApiFilter.profile: GraphicsInfo.profile === GraphicsInfo.CoreProfile ?
                                                           GraphicsApiFilter.CoreProfile : GraphicsApiFilter.NoProfile
                            //指定tehcnique使用的渲染通道
                            renderPasses: RenderPass {
                                //着色器
                                shaderProgram: ShaderProgram {
                                    //GLSL可以用字符串或者文件
                                    vertexShaderCode: vertStr
                                    //vertexShaderCode: loadSource('qrc:/triangle1.vert')
                                    fragmentShaderCode: fragStr
                                    //fragmentShaderCode: loadSource('qrc:/triangle1.frag')
                                }
                            }
                        }
                    }

                }//end Material
                components: [geometry, material]
            }
        }
    }
    property string vertStr: '
                #version 330 core
                layout (location = 0) in vec3 position;
                void main()
                {
                    gl_Position = vec4(position, 1.0f);
                }
                '
    property string fragStr: '
                #version 330 core
                out vec4 color;
                void main()
                {
                    color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
                }
                '
}
