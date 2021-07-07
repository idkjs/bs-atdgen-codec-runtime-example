let event_file = "events.json";

let print_event = (event: Meetup_t.event) => {
  Js.log2("=== %s summary ===", event.name);
  Js.log2("date: %s", Js.Date.toUTCString(event.date));
  Js.log2(
    "access: %s",
    switch (event.access) {
    | `Private(_) => "private (registration required)"
    | `Public({address}) => "public (location: `" ++ address ++ "`)"
    },
  );
  Js.log4(
    "host: %s <%s>%s",
    event.host.name,
    event.host.email,
    switch (event.host.phone) {
    | None => ""
    | Some(p) => " (" ++ p ++ ")"
    },
  );
  Js.log2("guests: %d", List.length(event.guests));
};

let read_events = () => {
  /* read the file from disk */
  let file_content = Node.Fs.readFileAsUtf8Sync(event_file);
  /* parse the json */
  let json = Js.Json.parseExn(file_content);
  /* turn it into a proper record */
  let events: Meetup_t.events = (
    Atdgen_codec_runtime.Decode.decode(Meetup_bs.read_events, json): Meetup_t.events
  );
  events;
};

let write_events = events => {
  /* turn a list of records into json */
  let json =
    Atdgen_codec_runtime.Encode.encode(Meetup_bs.write_events, events);
  /* convert the json to string */
  let file_content = Js.Json.stringifyWithSpace(json, 2);
  /* write the json in our file */
  Node.Fs.writeFileAsUtf8Sync(event_file, file_content);
};

let print_events = () => read_events() |> List.iter(print_event);

let add_event = (name, email) => {
  let events = read_events();
  let host = {Meetup_t.name, email, phone: None};

  let new_event = {
    Meetup_t.access: `Public({address: "Central Park"}),
    name: "OCaml/Reason Meetup!",
    host,
    date: Js.Date.make(),
    guests: [host],
  };

  write_events([new_event, ...events]);
};

let () =
  switch (Array.to_list(Sys.argv)) {
  | [_, _, "print", ..._] => print_events()
  | [_, _, "add", name, email, ..._] => add_event(name, email)
  | _ => print_endline("usage: nodejs cli.bs.js <print|add>")
  };
