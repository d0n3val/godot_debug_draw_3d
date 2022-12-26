﻿#pragma once

#include "circular_buffer.h"
#include "colors.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#if defined(_MSC_VER)
#pragma warning(default : 4244)
#endif

#include <map>

using namespace godot;

class DataGraphManager;

class GraphParameters : public RefCounted {
	GDCLASS(GraphParameters, RefCounted);

public:
	enum GraphLinePosition : int {
		LINE_TOP = 0,
		LINE_CENTER = 1,
		LINE_BOTTOM = 2,
	};

	enum GraphPosition : int {
		POSITION_LEFT_TOP = 0,
		POSITION_RIGHT_TOP = 1,
		POSITION_LEFT_BOTTOM = 2,
		POSITION_RIGHT_BOTTOM = 3,
		POSITION_MAX
	};

	enum GraphSide : int {
		SIDE_LEFT = 0,
		SIDE_TOP = 1,
		SIDE_RIGHT = 2,
		SIDE_BOTTOM = 3,
		SIDE_MAX
	};

	enum TextFlags : int64_t {
		TEXT_CURRENT = 1 << 0,
		TEXT_AVG = 1 << 1,
		TEXT_MAX = 1 << 2,
		TEXT_MIN = 1 << 3,
		TEXT_ALL = TEXT_CURRENT | TEXT_AVG | TEXT_MAX | TEXT_MIN,
	};

	enum GraphType {
		GRAPH_NORMAL,
		GRAPH_FPS,
	};

private:
	/// Is Graph enabled
	bool enabled = true;
	/// Draw Graph title
	bool show_title = false;
	/// Draw a graph line aligned vertically in the center
	GraphLinePosition line_position = GraphLinePosition::LINE_CENTER;
	/// Sets the text visibility *TextFlags*
	BitField<TextFlags> show_text_flags = TextFlags::TEXT_ALL;
	/// The size of the graph.
	Vector2i size = Vector2i(256, 64);
	/// The size of the buffer where the values are stored.
	int buffer_size = 256;
	/// Offset from the corner selected in position
	Vector2i offset = Vector2i(8, 8);
	/// FPS Graph position *GraphPosition*
	GraphPosition corner = GraphPosition::POSITION_RIGHT_TOP;
	/// Graph line width
	real_t line_width = 1.0f;
	/// Graph line color
	Color line_color = Colors::orange_red;
	/// Background color
	Color background_color = Colors::gray_graph_bg;
	/// Border color
	Color border_color = Colors::black;

	/// Text suffix
	String text_suffix = "";
	/// Custom Font
	Ref<Font> custom_font = Ref<Font>();
	/// Title Size
	int title_size = 14;
	/// Text Size
	int text_size = 12;
	/// Color of the info title
	Color title_color = Colors::white_smoke;
	/// Color of the info text
	Color text_color = Colors::white_smoke;
	/// Float precision
	int text_precision = 2;
	/// Parent graph to stack with
	StringName parent_graph;
	/// Parent graph side to stack with
	GraphSide parent_graph_side = GraphSide::SIDE_BOTTOM;

protected:
	mutable std::recursive_mutex datalock;
	std::unique_ptr<CircularBuffer<double> > buffer_data;
	StringName title;
	DataGraphManager *owner = nullptr;

	static void _bind_methods();
	virtual void _update_received(double _value);
	void _init(DataGraphManager *_owner, StringName _title);

public:
	GraphParameters(){};
	GraphParameters(DataGraphManager *_owner, StringName _title);

	virtual GraphType get_type() { return GraphType::GRAPH_NORMAL; };
	StringName get_title() const;

	void set_enabled(const bool _state);
	bool is_enabled() const;
	void set_show_title(const bool _state);
	bool is_show_title() const;
	void set_line_position(const GraphLinePosition _position);
	GraphLinePosition get_line_position() const;
	void set_show_text_flags(const BitField<TextFlags> _flags);
	BitField<TextFlags> get_show_text_flags() const;
	void set_size(const Vector2i &_size);
	Vector2i get_size() const;
	void set_buffer_size(const int _buf_size);
	int get_buffer_size() const;
	void set_offset(const Vector2i &_offset);
	Vector2i get_offset() const;
	void set_corner(const GraphPosition _position);
	GraphPosition get_corner() const;
	void set_line_width(const real_t _width);
	real_t get_line_width() const;
	void set_line_color(const Color &_new_color);
	Color get_line_color() const;
	void set_background_color(const Color &_new_color);
	Color get_background_color() const;
	void set_border_color(const Color &_new_color);
	Color get_border_color() const;
	void set_text_suffix(const String &_suffix);
	String get_text_suffix() const;
	void set_custom_font(const Ref<Font> _custom_font);
	Ref<Font> get_custom_font() const;
	void set_title_size(const int _size);
	int get_title_size() const;
	void set_text_size(const int _size);
	int get_text_size() const;
	void set_title_color(const Color &_new_color);
	Color get_title_color() const;
	void set_text_color(const Color &_new_color);
	Color get_text_color() const;
	void set_text_precision(const int _precision);
	int get_text_precision() const;
	void set_parent_graph(const StringName &_graph);
	StringName get_parent_graph() const;
	void set_parent_graph_side(const GraphSide _side);
	GraphSide get_parent_graph_side() const;

	void set_parent(const StringName &_name, const GraphSide _side = GraphSide::SIDE_BOTTOM);

public:
	void update(double value);

	struct graph_rects {
		Rect2i full;
		Rect2i base;
	};

	Vector2i _get_graph_position(const bool &_is_root, const GraphParameters::GraphPosition &_corner, const GraphParameters::graph_rects &_rects) const;
	graph_rects draw(CanvasItem *_ci, const Ref<Font> &_font, const graph_rects &_prev_rects, const GraphPosition &_corner, const bool &_is_root) const;
};

VARIANT_ENUM_CAST(GraphParameters::GraphPosition);
VARIANT_ENUM_CAST(GraphParameters::GraphLinePosition);
VARIANT_ENUM_CAST(GraphParameters::GraphSide);
VARIANT_BITFIELD_CAST(GraphParameters::TextFlags);

class FPSGraphParameters : public GraphParameters {
	GDCLASS(FPSGraphParameters, GraphParameters);

public:
	/// Switch between frame time and FPS modes
	/// Only for FPS Graphs
	bool frametime_mode = true;

protected:
	bool is_ms = false;

	static void _bind_methods();
	virtual void _update_received(double _value) override;

public:
	FPSGraphParameters(){};
	FPSGraphParameters(DataGraphManager *_owner, StringName _title);

	virtual GraphType get_type() override { return GraphType::GRAPH_FPS; };
	void set_frame_time_mode(const bool _state);
	bool is_frame_time_mode() const;
};

class DataGraphManager {
	std::vector<Ref<GraphParameters> > graphs;
	mutable std::recursive_mutex datalock;
	class DebugDraw *owner = nullptr;

public:
	DataGraphManager(class DebugDraw *root);
	~DataGraphManager();

	void draw(CanvasItem *_ci, Ref<Font> _font, Vector2 _vp_size) const;
	Ref<GraphParameters> create_graph(const StringName &_title);
	Ref<GraphParameters> create_fps_graph(const StringName &_title);
	// TODO: add ability to just register callback to get data for graph.
	// like: set_graph_data_callback("randf", Callable(this, &"_get_graph_data"))
	void update_fps_graphs(double delta);
	void graph_update_data(const StringName &_title, const double &_data);
	void remove_graph(const StringName &_title);
	void clear_graphs();
	Ref<GraphParameters> get_graph_config(const StringName &_title) const;
	PackedStringArray get_graph_names() const;
};