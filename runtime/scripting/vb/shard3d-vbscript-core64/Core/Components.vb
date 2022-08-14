Imports Shard3D.Core
Imports System

Namespace Shard3D.Core
    Public MustInherit Class Component
        Public Property _Actor As Actor
    End Class
End Namespace

Namespace Shard3D.Components
    Public Class TransformComponent
        Inherits Component

        Public Property Translation As Vector3
            Get
                Dim _translation As Vector3 = Nothing
                InternalCalls.TransformComponent_GetTranslation(_Actor.ID, _translation)
                Return _translation
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.TransformComponent_SetTranslation(_Actor.ID, value)
            End Set
        End Property

        Public Property Rotation As Vector3
            Get
                Dim _rotation As Vector3 = Nothing
                InternalCalls.TransformComponent_GetRotation(_Actor.ID, _rotation)
                Return _rotation
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.TransformComponent_SetRotation(_Actor.ID, value)
            End Set
        End Property

        Public Property Scale As Vector3
            Get
                Dim _scale As Vector3 = Nothing
                InternalCalls.TransformComponent_GetScale(_Actor.ID, _scale)
                Return _scale
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.TransformComponent_SetScale(_Actor.ID, value)
            End Set
        End Property
    End Class

    Public Class CameraComponent
        Inherits Component

        Public Property ProjectionType As Core.CameraProjectionType
            Get
                Dim _pt As Integer = Nothing
                InternalCalls.CameraComponent_GetProjectionType(_Actor.ID, _pt)
                Return CType(_pt, CameraProjectionType)
            End Get
            Set(ByVal value As Core.CameraProjectionType)
                Dim val As Integer = CInt(value)
                InternalCalls.CameraComponent_SetProjectionType(_Actor.ID, val)
            End Set
        End Property

        Public Property FOV As Single
            Get
                Dim _fov As Single = Nothing
                InternalCalls.CameraComponent_GetFOV(_Actor.ID, _fov)
                Return _fov
            End Get
            Set(ByVal value As Single)
                InternalCalls.CameraComponent_SetFOV(_Actor.ID, value)
            End Set
        End Property

        Public Property NearClipPlane As Single
            Get
                Dim _nc As Single = Nothing
                InternalCalls.CameraComponent_GetNearClip(_Actor.ID, _nc)
                Return _nc
            End Get
            Set(ByVal value As Single)
                InternalCalls.CameraComponent_SetNearClip(_Actor.ID, value)
            End Set
        End Property

        Public Property FarClipPlane As Single
            Get
                Dim _fc As Single = Nothing
                InternalCalls.CameraComponent_GetFarClip(_Actor.ID, _fc)
                Return _fc
            End Get
            Set(ByVal value As Single)
                InternalCalls.CameraComponent_SetFarClip(_Actor.ID, value)
            End Set
        End Property

        Public Sub Possess()
            InternalCalls.SceneManager_PossessCameraActor(_Actor.ID)
        End Sub
    End Class

    Public Class AudioComponent
        Inherits Component

        Public Property File As String
            Get
                Dim _f As String = Nothing
                InternalCalls.AudioComponent_GetFile(_Actor.ID, _f)
                Return _f
            End Get
            Set(ByVal value As String)
                InternalCalls.AudioComponent_SetFile(_Actor.ID, value)
            End Set
        End Property

        Public Property Properties As Core.AudioProperties
            Get
                Dim _ap As AudioProperties = New AudioProperties()
                InternalCalls.AudioComponent_GetPropertiesVolume(_Actor.ID, _ap.volume)
                InternalCalls.AudioComponent_GetPropertiesPitch(_Actor.ID, _ap.pitch)
                Return _ap
            End Get
            Set(ByVal value As Core.AudioProperties)
                InternalCalls.AudioComponent_SetPropertiesVolume(_Actor.ID, value.volume)
                InternalCalls.AudioComponent_SetPropertiesPitch(_Actor.ID, value.pitch)
            End Set
        End Property

        Public Sub Play()
            InternalCalls.AudioComponent_Play(_Actor.ID)
        End Sub

        Public Sub [Stop]()
            InternalCalls.AudioComponent_Stop(_Actor.ID)
        End Sub

        Public Sub Pause()
            InternalCalls.AudioComponent_Pause(_Actor.ID)
        End Sub

        Public Sub [Resume]()
            InternalCalls.AudioComponent_Resume(_Actor.ID)
        End Sub

        Public Sub Update()
            InternalCalls.AudioComponent_Update(_Actor.ID)
        End Sub
    End Class

    Public Class MeshComponent
        Inherits Component

        Public Property File As String
            Get
                Dim _f As String = Nothing
                InternalCalls.MeshComponent_GetFile(_Actor.ID, _f)
                Return _f
            End Get
            Set(ByVal value As String)
                InternalCalls.MeshComponent_SetFile(_Actor.ID, value)
            End Set
        End Property

        Public Sub Apply()
            InternalCalls.MeshComponent_Load(_Actor.ID)
        End Sub
    End Class

    Public Class BillboardComponent
        Inherits Component

        Public Property File As String
            Get
                Dim _f As String = Nothing
                InternalCalls.BillboardComponent_GetFile(_Actor.ID, _f)
                Return _f
            End Get
            Set(ByVal value As String)
                InternalCalls.BillboardComponent_SetFile(_Actor.ID, value)
            End Set
        End Property

        Public Sub Apply()
            InternalCalls.BillboardComponent_Load(_Actor.ID)
        End Sub
    End Class

    Public Class PointlightComponent
        Inherits Component

        Public Property Color As Vector3
            Get
                Dim _color As Vector3 = Nothing
                InternalCalls.PointlightComponent_GetColor(_Actor.ID, _color)
                Return _color
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.PointlightComponent_SetColor(_Actor.ID, value)
            End Set
        End Property

        Public Property Intensity As Single
            Get
                Dim _intensity As Single = Nothing
                InternalCalls.PointlightComponent_GetIntensity(_Actor.ID, _intensity)
                Return _intensity
            End Get
            Set(ByVal value As Single)
                InternalCalls.PointlightComponent_SetIntensity(_Actor.ID, value)
            End Set
        End Property

        Public Property Specular As Single
            Get
                Dim _specular As Single = Nothing
                InternalCalls.PointlightComponent_GetSpecularFactor(_Actor.ID, _specular)
                Return _specular
            End Get
            Set(ByVal value As Single)
                InternalCalls.PointlightComponent_SetSpecularFactor(_Actor.ID, value)
            End Set
        End Property

        Public Property AttenuationFactor As Vector3
            Get
                Dim _af As Vector3 = Nothing
                InternalCalls.PointlightComponent_GetAttenuationFactor(_Actor.ID, _af)
                Return _af
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.PointlightComponent_SetAttenuationFactor(_Actor.ID, value)
            End Set
        End Property
    End Class

    Public Class SpotlightComponent
        Inherits Component

        Public Property Color As Vector3
            Get
                Dim _color As Vector3 = Nothing
                InternalCalls.SpotlightComponent_GetColor(_Actor.ID, _color)
                Return _color
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.SpotlightComponent_SetColor(_Actor.ID, value)
            End Set
        End Property

        Public Property Intensity As Single
            Get
                Dim _intensity As Single = Nothing
                InternalCalls.SpotlightComponent_GetIntensity(_Actor.ID, _intensity)
                Return _intensity
            End Get
            Set(ByVal value As Single)
                InternalCalls.SpotlightComponent_SetIntensity(_Actor.ID, value)
            End Set
        End Property

        Public Property Specular As Single
            Get
                Dim _specular As Single = Nothing
                InternalCalls.SpotlightComponent_GetSpecularFactor(_Actor.ID, _specular)
                Return _specular
            End Get
            Set(ByVal value As Single)
                InternalCalls.SpotlightComponent_SetSpecularFactor(_Actor.ID, value)
            End Set
        End Property

        Public Property OuterAngle As Single
            Get
                Dim _angle As Single = Nothing
                InternalCalls.SpotlightComponent_GetOuterAngle(_Actor.ID, _angle)
                Return _angle
            End Get
            Set(ByVal value As Single)
                InternalCalls.SpotlightComponent_SetOuterAngle(_Actor.ID, value)
            End Set
        End Property

        Public Property InnerAngle As Single
            Get
                Dim _angle As Single = Nothing
                InternalCalls.SpotlightComponent_GetInnerAngle(_Actor.ID, _angle)
                Return _angle
            End Get
            Set(ByVal value As Single)
                InternalCalls.SpotlightComponent_SetInnerAngle(_Actor.ID, value)
            End Set
        End Property

        Public Property AttenuationFactor As Vector3
            Get
                Dim _af As Vector3 = Nothing
                InternalCalls.SpotlightComponent_GetAttenuationFactor(_Actor.ID, _af)
                Return _af
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.SpotlightComponent_SetAttenuationFactor(_Actor.ID, value)
            End Set
        End Property
    End Class

    Public Class DirectionalLightComponent
        Inherits Component

        Public Property Color As Vector3
            Get
                Dim _color As Vector3 = Nothing
                InternalCalls.DirectionalLightComponent_GetColor(_Actor.ID, _color)
                Return _color
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.DirectionalLightComponent_SetColor(_Actor.ID, value)
            End Set
        End Property

        Public Property Intensity As Single
            Get
                Dim _intensity As Single = Nothing
                InternalCalls.DirectionalLightComponent_GetIntensity(_Actor.ID, _intensity)
                Return _intensity
            End Get
            Set(ByVal value As Single)
                InternalCalls.DirectionalLightComponent_SetIntensity(_Actor.ID, value)
            End Set
        End Property

        Public Property Specular As Single
            Get
                Dim _specular As Single = Nothing
                InternalCalls.DirectionalLightComponent_GetSpecularFactor(_Actor.ID, _specular)
                Return _specular
            End Get
            Set(ByVal value As Single)
                InternalCalls.DirectionalLightComponent_SetSpecularFactor(_Actor.ID, value)
            End Set
        End Property
    End Class
End Namespace
