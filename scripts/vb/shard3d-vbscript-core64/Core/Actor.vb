Namespace Shard3D.Core
    Public Class Actor
        Protected Sub New()
            ID = 1
        End Sub

        Friend Sub New(ByVal _id As ULong)
            ID = _id
        End Sub

        Public ReadOnly ID As ULong
#Region "Transform"
        Public Property Translation As Vector3
            Get
                Dim _translation As Vector3 = Nothing
                InternalCalls.GetTranslation(ID, _translation)
                Return _translation
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.SetTranslation(ID, value)
            End Set
        End Property

        Public Property Rotation As Vector3
            Get
                Dim _rotation As Vector3 = Nothing
                InternalCalls.GetRotation(ID, _rotation)
                Return _rotation
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.SetRotation(ID, value)
            End Set
        End Property

        Public Property Scale As Vector3
            Get
                Dim _scale As Vector3 = Nothing
                InternalCalls.GetScale(ID, _scale)
                Return _scale
            End Get
            Set(ByVal value As Vector3)
                InternalCalls.SetScale(ID, value)
            End Set
        End Property
#End Region
#Region "ECS"
        Public Sub AddComponent(ByVal _component As Components)
            InternalCalls.ActorAddComponent(ID, _component)
        End Sub

        Public Sub RmvComponent(ByVal _component As Components)
            InternalCalls.ActorRmvComponent(ID, _component)
        End Sub
#End Region
    End Class
End Namespace
