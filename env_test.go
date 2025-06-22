package ginja_test

import (
	"reflect"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"
	"go.yuchanns.xyz/ginja"
)

type Suite struct {
	env *ginja.Environment
}

func (s *Suite) Setup() (err error) {
	env, err := ginja.New()
	if err != nil {
		return
	}
	s.env = env
	return
}

func (s *Suite) TearDown() {
	if s.env != nil {
		s.env.Close()
	}
}

func TestSuite(t *testing.T) {
	suite := &Suite{}

	err := suite.Setup()
	require.Nil(t, err)

	t.Cleanup(suite.TearDown)

	tt := reflect.TypeOf(suite)
	for i := range tt.NumMethod() {
		method := tt.Method(i)
		testFunc, ok := method.Func.Interface().(func(*Suite, *require.Assertions))
		if !ok {
			continue
		}
		t.Run(strings.TrimLeft(method.Name, "Test"), func(t *testing.T) {
			t.Parallel()

			testFunc(suite, require.New(t))
		})
	}

}

func (s *Suite) TestRenderTemplate(assert *require.Assertions) {
	env := s.env

	assert.Nil(env.AddTemplate("test_template", "Hello, {{ name }}!"))
	result, err := env.RenderTemplate("test_template", map[string]any{
		"name": "World",
	})
	assert.Nil(err)
	assert.Equal("Hello, World!", result)
}

