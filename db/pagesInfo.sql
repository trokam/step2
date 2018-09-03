-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler  version: 0.8.1
-- PostgreSQL version: 9.4
-- Project Site: pgmodeler.com.br
-- Model Author: ---


-- Database creation must be done outside an multicommand file.
-- These commands were put in this file only for convenience.
-- -- object: new_database | type: DATABASE --
-- -- DROP DATABASE IF EXISTS new_database;
-- CREATE DATABASE new_database
-- ;
-- -- ddl-end --
-- 

-- object: public.sequence | type: TABLE --
-- DROP TABLE IF EXISTS public.sequence CASCADE;
CREATE TABLE public.sequence(
	index serial NOT NULL,
	value varchar(1000) NOT NULL,
	count integer NOT NULL DEFAULT 0,
	modified integer NOT NULL DEFAULT 0,
	CONSTRAINT text_pkey PRIMARY KEY (index),
	CONSTRAINT value_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.sequence OWNER TO postgres;
-- ddl-end --

-- object: public.page | type: TABLE --
-- DROP TABLE IF EXISTS public.page CASCADE;
CREATE TABLE public.page(
	index serial NOT NULL,
	index_protocol smallint,
	index_domain integer,
	path varchar(2047), -- máximo según RFC https://tools.ietf.org/html/rfc3986
	level integer NOT NULL,
	processing bool NOT NULL,
	crunched integer NOT NULL,
	state smallint NOT NULL,
	type smallint NOT NULL,
	inserted integer NOT NULL,
	CONSTRAINT page_pkey PRIMARY KEY (index)

);
-- ddl-end --
ALTER TABLE public.page OWNER TO postgres;
-- ddl-end --

-- object: public.page_seq | type: TABLE --
-- DROP TABLE IF EXISTS public.page_seq CASCADE;
CREATE TABLE public.page_seq(
	index serial NOT NULL,
	index_page integer,
	index_sequence integer,
	seq_count integer NOT NULL,
	relevance_in_body smallint NOT NULL,
	relevance_in_url smallint NOT NULL,
	relevance_in_title smallint NOT NULL,
	relevance_total integer NOT NULL
);
-- ddl-end --
ALTER TABLE public.page_seq OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: sequence_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS sequence_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT sequence_fk FOREIGN KEY (index_sequence)
REFERENCES public.sequence (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: public.trait | type: TABLE --
-- DROP TABLE IF EXISTS public.trait CASCADE;
CREATE TABLE public.trait(
	index_page integer,
	title text NOT NULL, -- El texto es ilimitado para gran parte de los navegadores, varchar es menos efectivo
	content_size integer NOT NULL,
	complexity float NOT NULL,
	forwarded varchar(1000) NOT NULL
);
-- ddl-end --
ALTER TABLE public.trait OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: trait_uq | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS trait_uq CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT trait_uq UNIQUE (index_page);
-- ddl-end --

-- object: public.domain | type: TABLE --
-- DROP TABLE IF EXISTS public.domain CASCADE;
CREATE TABLE public.domain(
	index serial NOT NULL,
	value varchar(255) NOT NULL, -- máximo es 255 según RFC https://tools.ietf.org/html/rfc3986
	type smallint NOT NULL,
	CONSTRAINT domain_pkey PRIMARY KEY (index),
	CONSTRAINT domain_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.domain OWNER TO postgres;
-- ddl-end --

-- object: domain_fk | type: CONSTRAINT --
-- ALTER TABLE public.page DROP CONSTRAINT IF EXISTS domain_fk CASCADE;
ALTER TABLE public.page ADD CONSTRAINT domain_fk FOREIGN KEY (index_domain)
REFERENCES public.domain (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: sequence_value_index | type: INDEX --
-- DROP INDEX IF EXISTS public.sequence_value_index CASCADE;
CREATE INDEX sequence_value_index ON public.sequence
	USING btree
	(
	  value ASC NULLS LAST
	);
-- ddl-end --

-- object: public.protocol | type: TABLE --
-- DROP TABLE IF EXISTS public.protocol CASCADE;
CREATE TABLE public.protocol(
	index smallint NOT NULL,
	value varchar(10) NOT NULL,
	CONSTRAINT protocol_pkey PRIMARY KEY (index)

);
-- ddl-end --
ALTER TABLE public.protocol OWNER TO postgres;
-- ddl-end --

-- object: protocol_fk | type: CONSTRAINT --
-- ALTER TABLE public.page DROP CONSTRAINT IF EXISTS protocol_fk CASCADE;
ALTER TABLE public.page ADD CONSTRAINT protocol_fk FOREIGN KEY (index_protocol)
REFERENCES public.protocol (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: page_unique | type: CONSTRAINT --
-- ALTER TABLE public.page DROP CONSTRAINT IF EXISTS page_unique CASCADE;
ALTER TABLE public.page ADD CONSTRAINT page_unique UNIQUE (index_protocol,index_domain,path);
-- ddl-end --

-- object: page_path_index | type: INDEX --
-- DROP INDEX IF EXISTS public.page_path_index CASCADE;
CREATE INDEX page_path_index ON public.page
	USING btree
	(
	  path ASC NULLS LAST
	);
-- ddl-end --

-- object: domain_value_index | type: INDEX --
-- DROP INDEX IF EXISTS public.domain_value_index CASCADE;
CREATE INDEX domain_value_index ON public.domain
	USING btree
	(
	  value ASC NULLS LAST
	);
-- ddl-end --

-- object: page_seq_unique | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS page_seq_unique CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT page_seq_unique UNIQUE (index_page,index_sequence);
-- ddl-end --


