Namespace Shard3D.Core
    Public Class HUD
        Protected Sub New()
            ID = 1
        End Sub

        Friend Sub New(ByVal _id As ULong)
            ID = _id
        End Sub

        Public ReadOnly ID As ULong

        Public Property Position As Vector2
            Get
                Return New Vector2()
            End Get
            Set(ByVal value As Vector2)
            End Set
        End Property

        Public Property Rotation As Single
            Get
                Return New Single()
            End Get
            Set(ByVal value As Single)
            End Set
        End Property

        Public Property Scale As Vector2
            Get
                Return New Vector2()
            End Get
            Set(ByVal value As Vector2)
            End Set
        End Property

        Public Property DefaultTexture As String
            Get
                Return ""
            End Get
            Set(ByVal value As String)
            End Set
        End Property

        Public Property HoverTexture As String
            Get
                Return ""
            End Get
            Set(ByVal value As String)
            End Set
        End Property

        Public Property PressTexture As String
            Get
                Return ""
            End Get
            Set(ByVal value As String)
            End Set
        End Property
    End Class
End Namespace
