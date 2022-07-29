Namespace Shard3D.Core
    Public Class Actor
        Protected Sub New()
            ID = 1
        End Sub

        Friend Sub New(ByVal _id As ULong)
            ID = _id
            InternalCalls.Log($"Constructed Actor in VB {Me.ID}", LogSeverity.Warn)
        End Sub

        Public ReadOnly ID As ULong

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
    End Class
End Namespace
