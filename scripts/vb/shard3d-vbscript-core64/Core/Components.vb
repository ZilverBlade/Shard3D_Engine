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
    End Class
End Namespace
