#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <babeltrace/babeltrace.h>

static void record_event(struct bt_ctf_stream *stream,
		struct bt_ctf_event_class *event_class, uint64_t addr,
		uint64_t call_site) {
	int retval;

	struct bt_ctf_event *event = bt_ctf_event_create(event_class);
	assert(event);

	struct bt_ctf_field *payload_addr = bt_ctf_event_get_payload(event,
			"_addr");
	assert(payload_addr);
	retval = bt_ctf_field_unsigned_integer_set_value(payload_addr, addr);
	assert(retval == 0);
	BT_PUT(payload_addr);

	struct bt_ctf_field *payload_call_site = bt_ctf_event_get_payload(event,
			"_call_site");
	assert(payload_call_site);
	retval = bt_ctf_field_unsigned_integer_set_value(payload_call_site,
			call_site);
	assert(retval == 0);
	BT_PUT(payload_call_site);

	struct bt_ctf_field *context = bt_ctf_event_get_stream_event_context(
			event);
	assert(context);
	struct bt_ctf_field *context_procname =
			bt_ctf_field_structure_get_field_by_name(context, "_procname");
	assert(context_procname);
	const char procname[] = "myproc";
	for (int i = 0; i < 17; i++) {
		struct bt_ctf_field *fi = bt_ctf_field_array_get_field(
				context_procname, i);
		assert(fi);
		retval = bt_ctf_field_signed_integer_set_value(fi,
				i >= sizeof(procname) ? 0 : procname[i]);
		assert(retval == 0);
		BT_PUT(fi);
	}
	BT_PUT(context_procname);

	struct bt_ctf_field *context_vtid =
			bt_ctf_field_structure_get_field_by_name(context, "_vtid");
	assert(context_vtid);
	retval = bt_ctf_field_signed_integer_set_value(context_vtid, 1234);
	assert(retval == 0);
	BT_PUT(context_vtid);
	BT_PUT(context);



	retval = bt_ctf_stream_append_event(stream, event);
	assert(retval == 0);
	BT_PUT(event);
}

int main(void) {

	int retval;
	const char *trace_path = "/tmp/xyz";

	printf("trace path: %s\n", trace_path);

	// Create a writer
	struct bt_ctf_writer *writer = bt_ctf_writer_create(trace_path);
	assert(writer);

	// Create & configure clock...
	struct bt_ctf_clock *clock = bt_ctf_clock_create("monotonic");
	assert(clock);

	retval = bt_ctf_clock_set_description(clock, "Monotonic Clock");
	assert(retval == 0);
	retval = bt_ctf_clock_set_frequency(clock, 1000000000);
	assert(retval == 0);
	retval = bt_ctf_clock_set_offset(clock, 12345678);
	assert(retval == 0);
	// And add clock to writer
	retval = bt_ctf_writer_add_clock(writer, clock);
	assert(retval == 0);

	// Add any environment fields
	retval = bt_ctf_writer_add_environment_field(writer, "hostname",
			"loki-ust");
	assert(retval == 0);
	retval = bt_ctf_writer_add_environment_field(writer, "domain", "ust");
	assert(retval == 0);
	retval = bt_ctf_writer_add_environment_field(writer, "tracer_name",
			"lttng-ust");
	assert(retval == 0);
	retval = bt_ctf_writer_add_environment_field_int64(writer, "tracer_major",
			2);
	assert(retval == 0);
	retval = bt_ctf_writer_add_environment_field_int64(writer, "tracer_minor",
			3);
	assert(retval == 0);

	struct bt_ctf_stream_class *stream_class = bt_ctf_stream_class_create(
			"my_stream");
	assert(stream_class);

	retval = bt_ctf_stream_class_set_clock(stream_class, clock);
	assert(retval == 0);

	struct bt_ctf_field_type* event_context_type =
			bt_ctf_field_type_structure_create();
	assert(event_context_type);

	struct bt_ctf_field_type *procname_char_field_decl =
			bt_ctf_field_type_integer_create(8);
	assert(procname_char_field_decl);

	retval = bt_ctf_field_type_integer_set_is_signed(procname_char_field_decl,
			1);
	assert(retval == 0);

	retval = bt_ctf_field_type_set_alignment(procname_char_field_decl, 8);
	assert(retval == 0);
	retval = bt_ctf_field_type_integer_set_encoding(procname_char_field_decl,
			BT_CTF_STRING_ENCODING_UTF8);
	assert(retval == 0);

	struct bt_ctf_field_type* procname_string_field_decl =
			bt_ctf_field_type_array_create(procname_char_field_decl, 17);
	assert(procname_string_field_decl);
	retval = bt_ctf_field_type_structure_add_field(event_context_type,
			procname_string_field_decl, "_procname");
	assert(retval == 0);
	BT_PUT(procname_string_field_decl);

	struct bt_ctf_field_type *vtid_field_decl =
			bt_ctf_field_type_integer_create(32);
	assert(vtid_field_decl);

	retval = bt_ctf_field_type_integer_set_is_signed(vtid_field_decl, 1);
	assert(retval == 0);

	retval = bt_ctf_field_type_set_alignment(vtid_field_decl, 8);
	assert(retval == 0);
	retval = bt_ctf_field_type_structure_add_field(event_context_type,
			vtid_field_decl, "_vtid");
	assert(retval == 0);
	BT_PUT(vtid_field_decl);

	retval = bt_ctf_stream_class_set_event_context_type(stream_class,
			event_context_type);
	assert(retval == 0);
	BT_PUT(event_context_type);

	struct bt_ctf_event_class *func_entry_event_class =
			bt_ctf_event_class_create("lttng_ust_cyg_profile:func_entry");
	assert(func_entry_event_class);

	struct bt_ctf_field_type *addr_field_decl =
			bt_ctf_field_type_integer_create(64);
	assert(addr_field_decl);

	retval = bt_ctf_field_type_integer_set_is_signed(addr_field_decl, 0);
	assert(retval == 0);

	retval = bt_ctf_field_type_set_alignment(addr_field_decl, 8);
	assert(retval == 0);

	retval = bt_ctf_event_class_add_field(func_entry_event_class,
			addr_field_decl, "_addr");
	assert(retval == 0);
	retval = bt_ctf_event_class_add_field(func_entry_event_class,
			addr_field_decl, "_call_site");
	assert(retval == 0);

	retval = bt_ctf_stream_class_add_event_class(stream_class,
			func_entry_event_class);
	assert(retval == 0);

	struct bt_ctf_event_class *func_exit_event_class =
			bt_ctf_event_class_create("lttng_ust_cyg_profile:func_exit");
	assert(func_exit_event_class);

	retval = bt_ctf_event_class_add_field(func_exit_event_class,
			addr_field_decl, "_addr");
	assert(retval == 0);
	retval = bt_ctf_event_class_add_field(func_exit_event_class,
			addr_field_decl, "_call_site");
	assert(retval == 0);
	BT_PUT(addr_field_decl);

	retval = bt_ctf_stream_class_add_event_class(stream_class,
			func_exit_event_class);
	assert(retval == 0);

	struct bt_ctf_stream *stream = bt_ctf_writer_create_stream(writer,
			stream_class);
	assert(stream);
	BT_PUT(stream_class);
	BT_PUT(writer);

	bt_ctf_clock_set_time(clock, 10000000);
	record_event(stream, func_entry_event_class, 0x1000, 0x55000);
	bt_ctf_clock_set_time(clock, 20000000);
	record_event(stream, func_entry_event_class, 0x2000, 0x1050);
	bt_ctf_clock_set_time(clock, 30000000);
	record_event(stream, func_exit_event_class, 0x2000, 0x1050);
	bt_ctf_clock_set_time(clock, 40000000);
	record_event(stream, func_exit_event_class, 0x1000, 0x55000);
	BT_PUT(clock);

	BT_PUT(func_entry_event_class);
	BT_PUT(func_exit_event_class);

	retval = bt_ctf_stream_flush(stream);
	assert(retval == 0);
	BT_PUT(stream);

	return 0;
}
